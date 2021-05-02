#include "object.h"


namespace abel {

bool PackedInstruction::setPrefixes(const uint8_t new_prefixes[4]) {  // The unused ones have to be null
    unsigned cur = flags.getPrefCnt();

    for (unsigned i = 0; i < 4; ++i) {
        if (!new_prefixes[i])
            continue;

        if (cur >= 4) {
            ERR("Too many instruction prefixes");

            return true;
        }

        prefixes[cur++] = new_prefixes[i];
    }

    flags.setPrefCnt(cur);

    return false;
}

bool PackedInstruction::setRawOp(const uint8_t new_op[3], unsigned opcodeSize) {
    if (flags.getType() != T_PLAIN) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (opcodeSize > 3) {
        ERR("Opcode too big");
        return true;
    }

    flags.setOpcodeSize(opcodeSize);
    memcpy(rawOp, new_op, opcodeSize);

    return false;
}

bool PackedInstruction::setRexOp(const uint8_t new_op[3], unsigned opcodeSize) {
    if (flags.getType() != T_REX) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (opcodeSize > 3) {
        ERR("Opcode too big");
        return true;
    }

    flags.setOpcodeSize(opcodeSize);
    memcpy(rex.op, new_op, opcodeSize);

    return false;
}

bool PackedInstruction::ctor() {
    // TODO: ?

    return false;
}

void PackedInstruction::dtor() {
}

void PackedInstruction::hexDump() const {
    for (unsigned i = 0; i < flags.getPrefCnt(); ++i) {
        printf("%02x ", prefixes[i]);
    }

    switch (flags.getType()) {
    case T_PLAIN:
        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02x", rawOp[i]);
        }

        break;

    case T_REX:
        printf("%02x ", rex.rex);

        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02x", rex.op[i]);
        }

        break;

    case T_VEX2:
        break;

    case T_VEX3:
        break;

    default:
        ERR("Wrong instruction type");
        abort();
    }
    printf(" ");

    if (flags.hasModrm()) {
        printf("%02x ", modrm.full);
    }

    if (flags.hasSib()) {
        assert(flags.hasModrm());
        printf("%02x ", sib.full);
    }

    #define ITH_BYTE_(NUM, I)  (unsigned)((NUM >> (I * 8)) & 0xFF)

    switch (flags.getDispSize()) {
    case -1u:
        break;

    case 0:
        printf("%02x ", ITH_BYTE_(displacement, 0));
        break;

    case 1:
        printf("%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1));
        break;

    case 2:
        printf("%02x%02x%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1),
                                    ITH_BYTE_(displacement, 2), ITH_BYTE_(displacement, 3));
        break;

    case 3:
        printf("%02x%02x%02x%02x%02x%02x%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1),
                                                    ITH_BYTE_(displacement, 2), ITH_BYTE_(displacement, 3),
                                                    ITH_BYTE_(displacement, 4), ITH_BYTE_(displacement, 5),
                                                    ITH_BYTE_(displacement, 6), ITH_BYTE_(displacement, 7));
        break;

    default:
        ERR("Wrong displacement size");
        abort();
    }

    switch (flags.getImmSize()) {
    case -1u:
        break;

    case 0:
        printf("%02x ", ITH_BYTE_(immediate, 0));
        break;

    case 1:
        printf("%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1));
        break;

    case 2:
        printf("%02x%02x%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1),
                                    ITH_BYTE_(immediate, 2), ITH_BYTE_(immediate, 3));
        break;

    case 3:
        printf("%02x%02x%02x%02x%02x%02x%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1),
                                                    ITH_BYTE_(immediate, 2), ITH_BYTE_(immediate, 3),
                                                    ITH_BYTE_(immediate, 4), ITH_BYTE_(immediate, 5),
                                                    ITH_BYTE_(immediate, 6), ITH_BYTE_(immediate, 7));
        break;

    default:
        ERR("Wrong immediate size");
        abort();
    }

    #undef ITH_BYTE_

    printf("\n");
}

}

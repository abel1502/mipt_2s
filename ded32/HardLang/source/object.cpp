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
        printf("%02X ", prefixes[i]);
    }

    switch (flags.getType()) {
    case T_PLAIN:
        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02X", rawOp[i]);
        }

        break;

    case T_REX:
        printf("%02X ", rex.rex);

        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02X", rex.op[i]);
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
        printf("%02X ", modrm.full);
    }

    if (flags.hasSib()) {
        assert(flags.hasModrm());
        printf("%02X ", sib.full);
    }

    switch (flags.getDispSize()) {
    case 0:
        break;

    case 1:
        printf("%02X ",    (uint8_t) displacement);
        break;

    case 2:
        printf("%04X ",    (uint16_t)displacement);
        break;

    case 4:
        printf("%08X ",    (uint32_t)displacement);
        break;

    case 8:
        printf("%016llX ", (uint64_t)displacement);
        break;

    default:
        ERR("Wrong displacement size");
        abort();
    }

    switch (flags.getImmSize()) {
    case 0:
        break;

    case 1:
        printf("%02X ",    (uint8_t) immediate);
        break;

    case 2:
        printf("%04X ",    (uint16_t)immediate);
        break;

    case 4:
        printf("%08X ",    (uint32_t)immediate);
        break;

    case 8:
        printf("%016llX ", (uint64_t)immediate);
        break;

    default:
        ERR("Wrong immediate size");
        abort();
    }

    printf("\n");
}

}

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

bool PackedInstruction::compile(char **dest) const {
    char *oldDest = *dest;

    #define NEXT_BYTE_  *(*dest)++

    for (unsigned i = 0; i < flags.getPrefCnt(); ++i) {
        NEXT_BYTE_ = prefixes[i];
    }

    switch (flags.getType()) {
    case T_PLAIN:
        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            NEXT_BYTE_ = rawOp[i];
        }

        break;

    case T_REX:
        NEXT_BYTE_ = rex.rex;

        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            NEXT_BYTE_ = rex.op[i];
        }

        break;

    case T_VEX2:
    case T_VEX3:
        ERR("Can't yet compile VEX instructions");

        goto err;

    default:
        ERR("Wrong instruction type");
        abort();
    }

    if (flags.hasModrm()) {
        NEXT_BYTE_ = modrm.full;
    }

    if (flags.hasSib()) {
        assert(flags.hasModrm());

        NEXT_BYTE_ = sib.full;
    }

    #define ITH_BYTE_(NUM, I)  (unsigned)((NUM >> (I * 8)) & 0xFF)

    if (flags.getDispSize() != -1u) {
        unsigned dispSize = 1 << flags.getDispSize();

        for (unsigned i = 0; i < dispSize; ++i) {
            NEXT_BYTE_ = ITH_BYTE_(displacement, i);
        }
    }

    if (flags.getImmSize() != -1u) {
        unsigned immSize = 1 << flags.getImmSize();

        for (unsigned i = 0; i < immSize; ++i) {
            NEXT_BYTE_ = ITH_BYTE_(immediate, i);
        }
    }

    #undef ITH_BYTE_

    #undef NEXT_BYTE_

    return false;

err:
    *dest = oldDest;
    return true;
}

//================================================================================

ObjectFactory::result_e ObjectFactory::ctor() {
    lastResult = R_OK;

    if (code.ctor()) {
        ERR("Couldn't init underlying Vector");
        return lastResult = R_BADMEMORY;
    }

    nextLabelIdx = 0;
    stkCurTos = 0;
    stkCurSize = 0;

    return lastResult = R_OK;
}

void ObjectFactory::dtor() {
    code.dtor();
}

reg_e ObjectFactory::stkTos(unsigned depth) const {
    REQUIRE(depth <= stkCurSize);

    return REGSTK_REGS[(REGSTK_SIZE + stkCurTos - depth) % REGSTK_SIZE];
}

ObjectFactory::result_e ObjectFactory::stkPush() {
    if (stkCurSize == REGSTK_SIZE) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::add_rm64_imm32)
                      .setRmReg(REG_BP)
                      .setImm(8 * REGSTK_SIZE / 2);

        for (unsigned i = 0; i < REGSTK_SIZE / 2; ++i) {
            TRY(addInstr());
            getLastInstr().setOp(Opcode_e::mov_rm64_r64)
                          .setRmMemReg(REG_BP, Instruction::mode_t::DISP_8)
                          .setR(stkTos(REGSTK_SIZE - i))
                          .setDisp(8 * (i - REGSTK_SIZE / 2));
        }

        stkCurSize = REGSTK_SIZE / 2;
    }

    stkCurTos = (stkCurTos + 1) % REGSTK_SIZE;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkPop() {
    if (stkCurSize == 0) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::sub_rm64_imm32)
                      .setRmReg(REG_BP)
                      .setImm(8 * REGSTK_SIZE / 2);

        for (unsigned i = 0; i < REGSTK_SIZE / 2; ++i) {
            TRY(addInstr());
            getLastInstr().setOp(Opcode_e::mov_r64_rm64)
                          .setRmMemReg(REG_BP, Instruction::mode_t::DISP_8)
                          .setR(stkTos(REGSTK_SIZE - i))
                          .setDisp(8 * (i - REGSTK_SIZE / 2));
        }

        stkCurSize = REGSTK_SIZE / 2;
    }

    stkCurTos = (REGSTK_SIZE + stkCurTos - 1) % REGSTK_SIZE;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkFlush() {
    if (stkCurSize == 0)
        return lastResult = R_OK;

    TRY(addInstr());
    getLastInstr().setOp(Opcode_e::add_rm64_imm32)
                  .setRmReg(REG_BP)
                  .setImm(8 * stkCurSize);

    for (unsigned i = 0; i < stkCurSize; ++i) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::mov_rm64_r64)
                      .setRmMemReg(REG_BP, Instruction::mode_t::DISP_8)
                      .setR(stkTos(stkCurSize - i))
                      .setDisp(8 * (i - stkCurSize));
    }

    stkCurSize = 0;
    stkCurTos = 0;  // This allows to use flushing as a way to align computation stacks between two separate code segements

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkPull(unsigned req) {
    if (stkCurSize >= req)
        return lastResult = R_OK;

    TRY(addInstr());
    getLastInstr().setOp(Opcode_e::sub_rm64_imm32)
                  .setRmReg(REG_BP)
                  .setImm(8 * (req - stkCurSize));

    for (; stkCurSize < req; ++stkCurSize) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::mov_r64_rm64)
                      .setRmMemReg(REG_BP, Instruction::mode_t::DISP_8)
                      .setR(stkTos(stkCurSize))
                      .setDisp(-8 * stkCurSize);
    }

    return lastResult = R_OK;
}

unsigned ObjectFactory::reserveLabel() {
    return nextLabelIdx++;
}

unsigned ObjectFactory::placeLabel() {
    unsigned label = reserveLabel();

    return placeLabel(label) ? -1u : label;
}

ObjectFactory::result_e ObjectFactory::placeLabel(unsigned reservedLabelIdx) {
    // TODO: Implement!

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::addInstr() {
    if (code.append() || code[-1].ctor()) {
        return lastResult = R_BADMEMORY;
    }

    return lastResult = R_OK;
}

Instruction &ObjectFactory::getLastInstr() {
    REQUIRE(code.getSize() > 0);

    return code[-1];
}

}

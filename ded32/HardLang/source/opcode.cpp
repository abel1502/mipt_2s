#include "opcode.h"

#include <initializer_list>
#include <cassert>


namespace abel {

bool Instruction::OperandR::writeModRm(modrm_t &modrm) const {
    modrm.reg = reg & 7;

    return false;
}

bool Instruction::OperandR::writeRex(rex_t &rex) const {
    rex.R = reg >= REG_8;

    return false;
}

bool Instruction::OperandRM::writeModRm(modrm_t &modrm, sib_t &sib, bool &hasSib) const {
    hasSib = false;

    switch (mode.mode) {
    case mode.MODE_REG:
        modrm.mod = 0b11;
        modrm.rm = reg & 7;

        return false;

    case mode.MODE_MEM_RIP:
        modrm.mod = 0b00;
        modrm.rm  = REG_BP;

        if (mode.disp != mode.DISP_32) {
            ERR("RIP-relative addressing requires a 32-bit displacement");
            return true;
        }

        return false;

    case mode.MODE_MEM_REG:
        modrm.mod = mode.disp;
        modrm.rm  = reg & 7;

        if (mode.disp == mode.DISP_NONE && reg == REG_BP) {
            ERR("RBP-relative addressing can't be used without a displacement");
            return true;
        }

        if ((reg & 7) == REG_SP) {
            // TODO: Maybe convert to sib automatically?
            ERR("RSP- and R13-relative addressing must use SIB instead");
            return true;
        }

        return false;

    case mode.MODE_MEM_SIB:
        modrm.mod = mode.disp;
        modrm.rm  = REG_SP;

        hasSib = true;

        sib.base =  mode.sib & mode.SIB_BASE  ? reg   : REG_BP;
        sib.index = mode.sib & mode.SIB_INDEX ? index : REG_SP;
        sib.scale = scale;

        return false;

    NODEFAULT
    }

    return true;
}

bool Instruction::OperandRM::writeRex(rex_t &rex) const {
    rex.B = reg >= REG_8;

    if (mode.mode == mode.MODE_MEM_SIB) {
        rex.X = index >= REG_8;
    }

    return false;
}

bool Instruction::ctor() {
    rm.mode  = {mode_t::MODE_MEM_SIB, 0, mode_t::DISP_NONE};
    rm.reg   = REG_BP;
    rm.index = REG_SP;
    rm.scale = SCALE_1;

    r.reg    = REG_A;

    imm.val_qu = 0;

    disp.val_qu = 0;
    TRY_B(disp.symbol.ctorNone());

    removed = false;

    TRY_B(symbolHere.ctorNone());

    return false;
}

void Instruction::dtor() {
    disp.symbol.dtor();
    symbolHere.dtor();
}

bool Instruction::compile(PackedInstruction &pi) const {
    TRY_B(pi.ctor());

    if (removed) {
        ERR("Can't compile removed commands");  // TODO: Maybe replace with nops instead?

        return true;
    }

    switch (op) {
        #include "opcodes.dslctx.h"

        #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX)                        \
            case Opcode_e::NAME: {                                                                              \
                static_assert(RMSIZE == -1 || DISPSIZE == -1);                                                  \
                                                                                                                \
                uint8_t prefixes[4] = REQPREFIX;                                                                \
                                                                                                                \
                constexpr unsigned opSize = std::initializer_list<uint8_t>BYTES.size();                         \
                uint8_t opBytes[3] = BYTES;                                                                     \
                                                                                                                \
                TRY_B(compile(pi, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, prefixes, opSize, opBytes));       \
            } return false;

        #include "opcodes.dsl.h"

        #undef OPDEF

    NODEFAULT
    }

    return true;
}

bool Instruction::compile(PackedInstruction &pi, unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize,
                          unsigned variant, uint8_t prefixes[4], unsigned opSize, uint8_t opBytes[3]) const {

    assert(!removed);

    TRY_B(pi.setPrefixes(prefixes));

    // TODO: Also handle VEX-es
    if (needsRex(rmSize, rSize, dispSize, immSize)) {
        pi.flags.setType(pi.T_REX); // TODO: Encapsulate!!
        pi.rex.magic = 0b0100;
        // TODO: Encapsulate
        pi.rex.W = rmSize == SIZE_Q || rSize == SIZE_Q || dispSize == SIZE_Q || immSize == SIZE_Q;

        TRY_B(pi.setRexOp(opBytes, opSize));
    } else {
        pi.flags.setType(pi.T_PLAIN);
        TRY_B(pi.setRawOp(opBytes, opSize));
    }

    if (rmSize != -1u) {
        bool hasSib = false;
        TRY_B(rm.writeModRm(pi.modrm, pi.sib, hasSib));

        pi.flags.setHasModrm(true);
        pi.flags.setHasSib(hasSib);

        if (pi.flags.getType() == pi.T_REX) {
            TRY_B(rm.writeRex(pi.rex));
        }
    }

    if (rSize != -1u) {
        if (pi.flags.hasModrm()) {
            TRY_B(r.writeModRm(pi.modrm));

            if (pi.flags.getType() == pi.T_REX) {
                TRY_B(r.writeRex(pi.rex));
            }
        } else if (pi.flags.getType() == pi.T_REX) {
            TRY_B(pi.setRexOpVariant(r.reg));
        } else {
            TRY_B(pi.setRawOpVariant(r.reg));
        }
    } else if (variant != -1u) {
        pi.flags.setHasModrm(true);  // TODO: Maybe assert instead of setting?

        pi.modrm.reg = variant;
    }

    if (dispSize != -1u) {  // TODO: Also handle displacement for sib!
        assert(rmSize == -1u);

        pi.flags.setDispSize(dispSize);
        pi.displacement = disp.val_qu;
    } else if (rmSize != -1u && rm.mode.disp != rm.mode.DISP_NONE) {
        assert(dispSize == -1u);

        switch (rm.mode.disp) {
        case rm.mode.DISP_8:
            pi.flags.setDispSize(0);
            break;

        case rm.mode.DISP_32:
            pi.flags.setDispSize(2);
            break;

        case rm.mode.DISP_NONE:
        NODEFAULT
        }

        pi.displacement = disp.val_qu;
    }

    if (immSize != -1u) {
        pi.flags.setImmSize(immSize);
        pi.immediate = imm.val_qu;
    }

    return false;
}

Instruction &Instruction::setOp(Opcode_e new_op) {
    op = new_op;

    return *this;
}

Instruction &Instruction::setRm(mode_t mode, reg_e reg, reg_e index, scale_e scale) {
    rm.mode = mode;
    rm.reg = reg;
    rm.index = index;
    rm.scale = scale;

    return *this;
}

Instruction &Instruction::setRmReg(reg_e reg) {
    return setRm({mode_t::MODE_REG, 0, mode_t::DISP_NONE}, reg, REG_A, SCALE_1);
}

Instruction &Instruction::setRmMemReg(reg_e reg, mode_t::disp_e dispMode) {
    return setRm({mode_t::MODE_MEM_REG, 0, dispMode}, reg, REG_A, SCALE_1);
}

Instruction &Instruction::setRmMemRip() {
    return setRm({mode_t::MODE_MEM_RIP, 0, mode_t::DISP_32}, REG_A, REG_A, SCALE_1);
}

Instruction &Instruction::setRmSib() {
    return setRm({mode_t::MODE_MEM_SIB, 0, mode_t::DISP_32}, REG_A, REG_A, SCALE_1);
}

Instruction &Instruction::setRmSib(reg_e base, mode_t::disp_e dispMode) {
    return setRm({mode_t::MODE_MEM_SIB, mode_t::SIB_BASE, dispMode}, base, REG_A, SCALE_1);
}

Instruction &Instruction::setRmSib(reg_e index, scale_e scale, mode_t::disp_e dispMode) {
    return setRm({mode_t::MODE_MEM_SIB, mode_t::SIB_INDEX, dispMode}, REG_A, index, scale);
}

Instruction &Instruction::setRmSib(reg_e base, reg_e index, scale_e scale, mode_t::disp_e dispMode) {
    return setRm({mode_t::MODE_MEM_SIB, mode_t::SIB_INDEX | mode_t::SIB_BASE, dispMode}, base, index, scale);
}

Instruction &Instruction::setR(reg_e reg) {
    r.reg = reg;

    return *this;
}

Instruction &Instruction::setDisp(int64_t value) {
    disp.val_q = value;

    return *this;
}

/*Instruction &Instruction::setDispLabel(unsigned idx) {
    REQUIRE(!disp.symbol.ctorLabel(idx));

    return *this;
}

Instruction &Instruction::setDispFunc(const Token *name) {
    REQUIRE(!disp.symbol.ctorFunction(name));

    return *this;
}

Instruction &Instruction::setDispFunc(const char *name, unsigned length) {
    REQUIRE(!disp.symbol.ctorFunction(name, length));

    return *this;
}*/

Instruction &Instruction::setImm(int64_t value) {
    imm.val_q = value;

    return *this;
}

unsigned Instruction::getLength() const {
    switch (op) {
        #include "opcodes.dslctx.h"

        #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX)                        \
            case Opcode_e::NAME: {                                                                              \
                static_assert(RMSIZE == -1 || DISPSIZE == -1);                                                  \
                                                                                                                \
                constexpr unsigned prefSize = std::initializer_list<uint8_t>REQPREFIX.size();                   \
                constexpr unsigned opSize = std::initializer_list<uint8_t>BYTES.size();                         \
                                                                                                                \
                return getLength(prefSize, opSize, RMSIZE, RSIZE, DISPSIZE, IMMSIZE);                                                                                        \
            };

        #include "opcodes.dsl.h"

        #undef OPDEF

    NODEFAULT
    }

    return -1;
}

unsigned Instruction::getLength(unsigned prefSize, unsigned opSize, unsigned rmSize,
                                unsigned rSize, unsigned dispSize, unsigned immSize) const {
    unsigned length = 0;

    length += prefSize + opSize;

    if (needsRex(rmSize, rSize, dispSize, immSize)) {
        length += 1;
    }

    length +=  rm.getLength(  rmSize)
           +    r.getLength(   rSize)
           + disp.getLength(dispSize)
           +  imm.getLength( immSize);

    if (rmSize != -1u) {
        switch (rm.mode.disp) {
        case rm.mode.DISP_NONE:
            break;

        case rm.mode.DISP_8:
            length += 1;
            break;

        case rm.mode.DISP_32:
            length += 4;
            break;

        NODEFAULT
        }
    }

    return length;
}

unsigned Instruction::getDispOffset(unsigned *ripOffset) const {
    switch (op) {
        #include "opcodes.dslctx.h"

        #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX)                        \
            case Opcode_e::NAME: {                                                                              \
                static_assert(RMSIZE == -1 || DISPSIZE == -1);                                                  \
                                                                                                                \
                constexpr unsigned prefSize = std::initializer_list<uint8_t>REQPREFIX.size();                   \
                constexpr unsigned opSize = std::initializer_list<uint8_t>BYTES.size();                         \
                                                                                                                \
                return getDispOffset(prefSize, ripOffset, opSize, RMSIZE, RSIZE, DISPSIZE, IMMSIZE);                                                                                        \
            };

        #include "opcodes.dsl.h"

        #undef OPDEF

    NODEFAULT
    }

    return -1;
}

unsigned Instruction::getDispOffset(unsigned prefSize, unsigned *ripOffset, unsigned opSize,
                                    unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize) const {
    unsigned length = 0;

    length += prefSize + opSize;

    if (needsRex(rmSize, rSize, dispSize, immSize)) {
        length += 1;
    }

    length +=  rm.getLength(  rmSize)
           +    r.getLength(   rSize);

    if (ripOffset) {
        *ripOffset = disp.getLength(dispSize)
                   +  imm.getLength( immSize);

        if (rmSize != -1u) {
            switch (rm.mode.disp) {
            case rm.mode.DISP_NONE:
                break;

            case rm.mode.DISP_8:
                *ripOffset += 1;
                break;

            case rm.mode.DISP_32:
                *ripOffset += 4;
                break;

            NODEFAULT
            }
        }
    }

    return length;
}

bool Instruction::needsRex(unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize) const {
    return (rm.usesRexReg((size_e)rmSize)) ||
           ( r.usesRexReg((size_e) rSize)) ||
           ((rmSize == SIZE_Q || rSize == SIZE_Q || dispSize == SIZE_Q || immSize == SIZE_Q) /* TODO: && not default 64 */);
}

}


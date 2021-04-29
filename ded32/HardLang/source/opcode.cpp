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

    default:
        break;
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
    //r.size   = SIZE_Q;

    //imm.size   = SIZE_Q;
    imm.val_qu = 0;

    //disp.size   = SIZE_Q;
    disp.val_qu = 0;

    return false;
}

void Instruction::dtor() {
}

bool Instruction::compile(PackedInstruction &pi) const {
    // TODO: !!!!!!!!!
    TRY_B(pi.ctor());

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
            } break;

        #include "opcodes.dsl.h"

        #undef OPDEF

    default:
        ERR("Unknown opcode.");
        abort();
    }

    return false;
}

// ded32 HardLang

bool Instruction::compile(PackedInstruction &pi, unsigned rmsize, unsigned rsize, unsigned dispsize, unsigned immsize,
                          unsigned variant, uint8_t prefixes[4], unsigned opSize, uint8_t opBytes[3]) const {

    TRY_B(pi.setPrefixes(prefixes));

    // TODO: Also handle VEX-es
    if ((rmsize != -1u && rm.usesRexReg()) ||
        (rsize  != -1u &&  r.usesRexReg()) ||
        ((rmsize == SIZE_Q || rsize == SIZE_Q || dispsize == SIZE_Q || immsize == SIZE_Q))) {
        // TODO: Also check that the default operand size isn't 64 for this operation

        pi.flags.setType(pi.T_REX); // TODO: Encapsulate!!
        pi.rex.magic = 0b0100;
        // TODO: Encapsulate
        pi.rex.W = rmsize == SIZE_Q || rsize == SIZE_Q || dispsize == SIZE_Q || immsize == SIZE_Q;

        TRY_B(pi.setRexOp(opBytes, opSize));
    } else {
        pi.flags.setType(pi.T_PLAIN);
        TRY_B(pi.setRawOp(opBytes, opSize));
    }

    if (rmsize != -1u) {
        bool hasSib = false;
        TRY_B(rm.writeModRm(pi.modrm, pi.sib, hasSib));

        pi.flags.setHasModrm(true);
        pi.flags.setHasSib(hasSib);

        if (pi.flags.getType() == pi.T_REX) {
            TRY_B(rm.writeRex(pi.rex));
        }
    }

    if (rsize != -1u) {
        if (!pi.flags.hasModrm()) {
            ERR("Embedded register operands not yet implemented, sorry");
            return true;
        }

        TRY_B(r.writeModRm(pi.modrm));

        if (pi.flags.getType() == pi.T_REX) {
            TRY_B(rm.writeRex(pi.rex));
        }
    } else if (variant != -1u) {
        pi.flags.setHasModrm(true);

        pi.modrm.reg = variant;
    }

    if (dispsize != -1u) {  // TODO: Also handle displacement for sib!
        pi.flags.setDispSize(1 << dispsize);
        pi.displacement = disp.val_qu;
    }

    if (immsize != -1u) {
        pi.flags.setImmSize(1 << immsize);
        pi.immediate = imm.val_qu;
    }

    return false;
}

}


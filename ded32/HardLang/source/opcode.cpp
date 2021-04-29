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

    #if 0

    case Opcode_e::NAME: {
        static_assert(RMSIZE == -1 || DISPSIZE == -1);

        uint8_t prefixes[4] = REQPREFIX;
        TRY_B(pi.setPrefixes(prefixes));

        constexpr unsigned opSize = std::initializer_list<uint8_t>BYTES.size();
        uint8_t opBytes[3] = BYTES;

        /* TODO: Also handle VEX-es */
        if ((RMSIZE != -1 && rm.usesRexReg()) ||
            (RSIZE  != -1 &&  r.usesRexReg()) ||
            ((RMSIZE == SIZE_Q || RSIZE == SIZE_Q || DISPSIZE == SIZE_Q || IMMSIZE == SIZE_Q))) {
            /* TODO: Also check that the default operand size isn't 64 for this operation */

            pi.flags.setType(pi.T_REX); /* TODO: Encapsulate!! */
            pi.rex.magic = 0b0100;
            /* TODO: Encapsulate */
            pi.rex.W = RMSIZE == SIZE_Q || RSIZE == SIZE_Q || DISPSIZE == SIZE_Q || IMMSIZE == SIZE_Q;

            TRY_B(pi.setRexOp(opBytes, opSize));
        } else {
            pi.flags.setType(pi.T_PLAIN);\
            TRY_B(pi.setRawOp(opBytes, opSize));
        }

        if constexpr (RMSIZE != -1) {
            bool hasSib = false;
            TRY_B(rm.writeModRm(pi.modrm, pi.sib, hasSib));

            pi.flags.setHasModrm(true);
            pi.flags.setHasSib(hasSib);

            if (pi.flags.getType() == pi.T_REX) {
                TRY_B(rm.writeRex(pi.rex));
            }
        }

        if constexpr (RSIZE != -1) {
            TRY_B(r.writeModRm(pi.modrm));

            pi.flags.setHasModrm(true);

            if (pi.flags.getType() == pi.T_REX) {
                TRY_B(rm.writeRex(pi.rex));
            }

            if constexpr (RMSIZE == -1) {
                ERR("Embedded register operands not yet implemented, sorry");
                return true;
            }
        }

        if constexpr (DISPSIZE != -1) {    /* TODO: Also handle displacement for sib! */
            pi.setDispSize(1 << DISPSIZE);
            pi.displacement = disp.val_qu;
        }

        if constexpr (IMMSIZE != -1) {
            pi.setImmSize(1 << IMMSIZE);
            pi.immediate = imm.val_qu;
        }

    } break;

    #endif // 0

    switch (op) {
        #include "opcodes.dslctx.h"

        #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX)                        \
            case Opcode_e::NAME: {                                                                              \
                static_assert(RMSIZE == -1 || DISPSIZE == -1);                                                  \
                                                                                                                \
                uint8_t prefixes[4] = REQPREFIX;                                                                \
                TRY_B(pi.setPrefixes(prefixes));                                                                \
                                                                                                                \
                constexpr unsigned opSize = std::initializer_list<uint8_t>BYTES.size();                         \
                uint8_t opBytes[3] = BYTES;                                                                     \
                                                                                                                \
                /* TODO: Also handle VEX-es */                                                                  \
                if ((RMSIZE != -1 && rm.usesRexReg()) ||                                                        \
                    (RSIZE  != -1 &&  r.usesRexReg()) ||                                                        \
                    ((RMSIZE == SIZE_Q || RSIZE == SIZE_Q || DISPSIZE == SIZE_Q || IMMSIZE == SIZE_Q))) {       \
                    /* TODO: Also check that the default operand size isn't 64 for this operation */            \
                                                                                                                \
                    pi.flags.setType(pi.T_REX); /* TODO: Encapsulate!!! */                                      \
                    pi.rex.magic = 0b0100;                                                                      \
                    /* TODO: Encapsulate */                                                                     \
                    pi.rex.W = RMSIZE == SIZE_Q || RSIZE == SIZE_Q || DISPSIZE == SIZE_Q || IMMSIZE == SIZE_Q;  \
                                                                                                                \
                    TRY_B(pi.setRexOp(opBytes, opSize));                                                        \
                } else {                                                                                        \
                    pi.flags.setType(pi.T_PLAIN);                                                               \
                                                                                                                \
                    TRY_B(pi.setRawOp(opBytes, opSize));                                                        \
                }                                                                                               \
                                                                                                                \
                if constexpr (RMSIZE != -1) {                                                                   \
                    bool hasSib = false;                                                                        \
                    TRY_B(rm.writeModRm(pi.modrm, pi.sib, hasSib));                                             \
                                                                                                                \
                    pi.flags.setHasSib(hasSib);                                                                 \
                    pi.flags.setHasModrm(true);                                                                 \
                                                                                                                \
                    if (pi.flags.getType() == pi.T_REX) {                                                       \
                        TRY_B(rm.writeRex(pi.rex));                                                             \
                    }                                                                                           \
                }                                                                                               \
                                                                                                                \
                if constexpr (RSIZE != -1) {                                                                    \
                    TRY_B(r.writeModRm(pi.modrm));                                                              \
                                                                                                                \
                    pi.flags.setHasModrm(true);                                                                 \
                                                                                                                \
                    if (pi.flags.getType() == pi.T_REX) {                                                       \
                        TRY_B(rm.writeRex(pi.rex));                                                             \
                    }                                                                                           \
                                                                                                                \
                    if constexpr (RMSIZE == -1) {                                                               \
                        ERR("Embedded register operands not yet implemented, sorry");                           \
                        return true;                                                                            \
                    }                                                                                           \
                }                                                                                               \
                                                                                                                \
                if constexpr (DISPSIZE != -1) {                                                                 \
                    pi.flags.setDispSize(1 << DISPSIZE);                                                        \
                    pi.displacement = disp.val_qu;                                                              \
                }                                                                                               \
                                                                                                                \
                if constexpr (IMMSIZE != -1) {                                                                  \
                    pi.flags.setImmSize(1 << IMMSIZE);                                                          \
                    pi.immediate = imm.val_qu;                                                                  \
                }                                                                                               \
                                                                                                                \
            } break;

        #include "opcodes.dsl.h"

        #undef OPDEF

    default:
        ERR("Unknown opcode.");
        abort();
    }

    return false;
}

}


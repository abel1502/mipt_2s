#ifndef OPCODE_H
#define OPCODE_H

#include "general.h"
#include "object.h"


namespace abel {

struct PackedInstruction;
union modrm_t;
union sib_t;
union rex_t;
union vex2_t;
union vex3_t;

enum class Opcode_e {
    #include "opcodes.dslctx.h"
    #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX) \
        NAME,

    #include "opcodes.dsl.h"

    #undef OPDEF
};

class Instruction {
public:
    enum reg_e {
        #undef REG_A
        #undef REG_C
        #undef REG_D
        #undef REG_B
        #undef REG_SP
        #undef REG_BP
        #undef REG_SI
        #undef REG_DI
        #undef REG_8
        #undef REG_9
        #undef REG_10
        #undef REG_11
        #undef REG_12
        #undef REG_13
        #undef REG_14
        #undef REG_15

        REG_A,
        REG_C,
        REG_D,
        REG_B,
        REG_SP,
        REG_BP,
        REG_SI,
        REG_DI,
        REG_8,
        REG_9,
        REG_10,
        REG_11,
        REG_12,
        REG_13,
        REG_14,
        REG_15,
    };

    enum size_e {
        #undef SIZE_B
        #undef SIZE_W
        #undef SIZE_D
        #undef SIZE_Q
        #undef SIZE_MMX
        #undef SIZE_XMM

        SIZE_B,
        SIZE_W,
        SIZE_D,
        SIZE_Q,
        SIZE_MMX,
        SIZE_XMM,
    };

    enum scale_e {
        #undef SCALE_1
        #undef SCALE_2
        #undef SCALE_4
        #undef SCALE_8

        SCALE_1 = 0b00,
        SCALE_2 = 0b01,
        SCALE_4 = 0b10,
        SCALE_8 = 0b11,
    };

    struct mode_t {
        enum mode_e {
            #undef MODE_REG
            #undef MODE_MEM_REG
            #undef MODE_MEM_RIP
            #undef MODE_MEM_SIB

            MODE_REG,
            MODE_MEM_REG,
            MODE_MEM_RIP,
            MODE_MEM_SIB,
        };

        enum sib_e {
            #undef SIB_BASE
            #undef SIB_INDEX

            SIB_BASE  = 0b01,
            SIB_INDEX = 0b10,
        };

        enum disp_e {
            #undef DISP_NONE
            #undef DISP_8
            #undef DISP_32

            DISP_NONE = 0b00,
            DISP_8    = 0b01,
            DISP_32   = 0b10,
        };

        mode_e mode;
        unsigned sib;
        disp_e disp;
    };

    struct OperandImm {
        union {
            int64_t  val_q;
            uint64_t val_qu;
        };
    };

    struct OperandDisp {
        union {
            int64_t  val_q;
            uint64_t val_qu;
        };
    };

    struct OperandR {
        reg_e reg;

        inline bool usesRexReg() const {
            return reg >= REG_8;
        }

        bool writeModRm(modrm_t &modrm) const;
        bool writeRex(rex_t &rex) const;
    };

    struct OperandRM {
        mode_t mode;

        reg_e reg;  // Also `base` when in SIB mode.
        reg_e index;
        scale_e scale;

        inline bool usesRexReg() const {
            return (reg >= REG_8 && (mode.mode <= mode.MODE_REG ||
                                     (mode.mode == mode.MODE_MEM_SIB && mode.sib & mode.SIB_BASE))) ||
                   (index >= REG_8 && mode.mode == mode.MODE_MEM_SIB && mode.sib & mode.SIB_INDEX);
        }

        bool writeModRm(modrm_t &modrm, sib_t &sib, bool &hasSib) const;
        bool writeRex(rex_t &rex) const;
    };

    //--------------------------------------------------------------------------------

    FACTORIES(Instruction)

    bool ctor();

    bool ctor(Opcode_e op);

    void dtor();


    void setOp(Opcode_e op);

    void setRM(mode_t mode, reg_e reg, reg_e index, scale_e scale);

    void setR(reg_e reg);

    void setDisp(uint64_t value);

    void setImm(uint64_t value);

    bool compile(PackedInstruction &pi) const;

//private:
    Opcode_e    op;

    OperandRM   rm;
    OperandR    r;
    OperandDisp disp;
    OperandImm  imm;

    bool compile(PackedInstruction &pi, unsigned rmsize, unsigned rsize, unsigned dispsize, unsigned immsize,
                 unsigned variant, uint8_t prefixes[4], unsigned opSize, uint8_t opBytes[3]) const;
};

}


#endif // OPCODE_H
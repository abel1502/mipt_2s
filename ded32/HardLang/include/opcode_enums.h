#ifndef OPCODE_ENUMS_H
#define OPCODE_ENUMS_H


namespace abel {

enum class Opcode_e {
    #include "opcodes.dslctx.h"
    #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX) \
        NAME,

    #include "opcodes.dsl.h"

    #undef OPDEF
};

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

}


#endif // OPCODE_ENUMS_H

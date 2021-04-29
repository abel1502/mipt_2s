#define OPDEF_BIN_ARITHM(NAME, BYTE_RM_R, BYTE_R_RM, BYTE_RM_IMM, IMM_VARIANT)                          \
    OPDEF(NAME##_rm8_r8,     {BYTE_RM_R - 1},   SIZE_B, SIZE_B,     -1,     -1,          -1, {})        \
    OPDEF(NAME##_rm16_r16,   {BYTE_RM_R},       SIZE_W, SIZE_W,     -1,     -1,          -1, {0x66})    \
    OPDEF(NAME##_rm32_r32,   {BYTE_RM_R},       SIZE_D, SIZE_D,     -1,     -1,          -1, {})        \
    OPDEF(NAME##_rm64_r64,   {BYTE_RM_R},       SIZE_Q, SIZE_Q,     -1,     -1,          -1, {})        \
                                                                                                        \
    OPDEF(NAME##_r8_rm8,     {BYTE_R_RM - 1},   SIZE_B, SIZE_B,     -1,     -1,          -1, {})        \
    OPDEF(NAME##_r16_rm16,   {BYTE_R_RM},       SIZE_W, SIZE_W,     -1,     -1,          -1, {0x66})    \
    OPDEF(NAME##_r32_rm32,   {BYTE_R_RM},       SIZE_D, SIZE_D,     -1,     -1,          -1, {})        \
    OPDEF(NAME##_r64_rm64,   {BYTE_R_RM},       SIZE_Q, SIZE_Q,     -1,     -1,          -1, {})        \
                                                                                                        \
    OPDEF(NAME##_rm8_imm8,   {BYTE_RM_IMM - 1}, SIZE_B,     -1,     -1, SIZE_B, IMM_VARIANT, {})        \
    OPDEF(NAME##_rm16_imm16, {BYTE_RM_IMM},     SIZE_W,     -1,     -1, SIZE_W, IMM_VARIANT, {0x66})    \
    OPDEF(NAME##_rm32_imm32, {BYTE_RM_IMM},     SIZE_D,     -1,     -1, SIZE_D, IMM_VARIANT, {})        \
    OPDEF(NAME##_rm64_imm32, {BYTE_RM_IMM},     SIZE_Q,     -1,     -1, SIZE_D, IMM_VARIANT, {})
    // The last one sign-extends!!!

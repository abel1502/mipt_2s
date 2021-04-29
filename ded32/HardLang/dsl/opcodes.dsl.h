/**
    #define OPDEF(NAME, BYTES, RMSIZE, RSIZE, DISPSIZE, IMMSIZE, VARIANT, REQPREFIX)

    NAME      - name for the enumeration
    BYTES     - a brace-enclosed list of opcode bytes
    RMSIZE    - size of R/M (SIZE_* enum or -1)
    RSIZE     - same for REG
    DISPSIZE  - same for DISP  (although it only refers to the explicitly referenced displacement, the sib and modrm may still require some)
    IMMSIZE   - same for IMM
    VARIANT   - -1 if REG field is used, or its constant value
    REQPREFIX - a brace-enclosed list of required prefixes (excluding REX and VEX)

**/


OPDEF_BIN_ARITHM(mov, 0x89, 0x8B, 0xC7, 0)


OPDEF(push_rm16,      {0xFF}, SIZE_W,     -1,     -1,     -1, -1, {0x66})
OPDEF(push_rm64,      {0xFF}, SIZE_Q,     -1,     -1,     -1, -1, {})

OPDEF(push_imm16,     {0x6A},     -1,     -1,     -1, SIZE_W,  6, {0x66})
OPDEF(push_imm32,     {0x68},     -1,     -1,     -1, SIZE_D,  6, {})
OPDEF(push_imm64,     {0x68},     -1,     -1,     -1, SIZE_Q,  6, {})


OPDEF(pop_rm16,       {0x8F}, SIZE_W,     -1,     -1,     -1, -1, {0x66})
OPDEF(pop_rm64,       {0x8F}, SIZE_Q,     -1,     -1,     -1, -1, {})


OPDEF(ret,            {0xC3},     -1,     -1,     -1,     -1, -1, {})
OPDEF(ret_imm16,      {0xC2},     -1,     -1,     -1, SIZE_W, -1, {})


OPDEF(call_rel32,     {0xE8},     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(call_rm64,      {0xFF}, SIZE_Q,     -1,     -1,     -1,  2, {})


OPDEF_BIN_ARITHM(xor, 0x31, 0x33, 0x81, 6)
OPDEF_BIN_ARITHM(or,  0x09, 0x0B, 0x81, 1)
OPDEF_BIN_ARITHM(and, 0x21, 0x23, 0x81, 4)
OPDEF_BIN_ARITHM(add, 0x01, 0x03, 0x81, 0)
OPDEF_BIN_ARITHM(sub, 0x29, 0x2B, 0x81, 5)


/*
OPDEF(mov_rm8_r8,     {0x88}, SIZE_B, SIZE_B,     -1,     -1, -1, {})
OPDEF(mov_rm16_r16,   {0x89}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(mov_rm32_r32,   {0x89}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(mov_rm64_r64,   {0x89}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(mov_r8_rm8,     {0x8A}, SIZE_B, SIZE_B,     -1,     -1, -1, {})
OPDEF(mov_r16_rm16,   {0x8B}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(mov_r32_rm32,   {0x8B}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(mov_r64_rm64,   {0x8B}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(mov_rm8_imm8,   {0xC6}, SIZE_B,     -1,     -1, SIZE_B,  0, {})
OPDEF(mov_rm16_imm16, {0xC7}, SIZE_W,     -1,     -1, SIZE_W,  0, {0x66})
OPDEF(mov_rm32_imm32, {0xC7}, SIZE_D,     -1,     -1, SIZE_D,  0, {})
OPDEF(mov_rm64_imm32, {0xC7}, SIZE_Q,     -1,     -1, SIZE_D,  0, {})  // Sign-extends!!!


OPDEF(xor_rm8_r8,     {0x30}, SIZE_B, SIZE_B,     -1,     -1, -1, {})
OPDEF(xor_rm16_r16,   {0x31}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(xor_rm32_r32,   {0x31}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(xor_rm64_r64,   {0x31}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(xor_r8_rm8,     {0x32}, SIZE_B, SIZE_B,     -1,     -1, -1, {})
OPDEF(xor_r16_rm16,   {0x33}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(xor_r32_rm32,   {0x33}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(xor_r64_rm64,   {0x33}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(xor_rm8_imm8,   {0x80}, SIZE_B,     -1,     -1, SIZE_B,  6, {})
OPDEF(xor_rm16_imm16, {0x81}, SIZE_W,     -1,     -1, SIZE_W,  6, {0x66})
OPDEF(xor_rm32_imm32, {0x81}, SIZE_D,     -1,     -1, SIZE_D,  6, {})
OPDEF(xor_rm64_imm32, {0x81}, SIZE_Q,     -1,     -1, SIZE_D,  6, {})  // Sign-extends!!!
*/




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


    TODO: a field to specify that I require a rex prefix or something like that

**/


OPDEF_BIN_COMMON(mov, 0x89, 0x8B, 0xC7, 0)


OPDEF_BIN_COMMON(xor, 0x31, 0x33, 0x81, 6)
OPDEF_BIN_COMMON(or,  0x09, 0x0B, 0x81, 1)
OPDEF_BIN_COMMON(and, 0x21, 0x23, 0x81, 4)
OPDEF_BIN_COMMON(add, 0x01, 0x03, 0x81, 0)
OPDEF_BIN_COMMON(sub, 0x29, 0x2B, 0x81, 5)
OPDEF_BIN_COMMON(cmp, 0x39, 0x3B, 0x81, 7)


OPDEF_UN_COMMON(not,  0xF7, 2)
OPDEF_UN_COMMON(neg,  0xF7, 3)
OPDEF_UN_COMMON(mul,  0xF7, 4)
OPDEF_UN_COMMON(imul, 0xF7, 5)
OPDEF_UN_COMMON(div,  0xF7, 6)
OPDEF_UN_COMMON(idiv, 0xF7, 7)


OPDEF(push_rm16,      {0xFF}, SIZE_W,     -1,     -1,     -1,  6, {0x66})
OPDEF(push_rm64,      {0xFF}, SIZE_Q,     -1,     -1,     -1,  6, {})

OPDEF(push_imm8,      {0x6A},     -1,     -1,     -1, SIZE_B, -1, {})
OPDEF(push_imm16,     {0x68},     -1,     -1,     -1, SIZE_W, -1, {0x66})
OPDEF(push_imm32,     {0x68},     -1,     -1,     -1, SIZE_D, -1, {})


OPDEF(pop_rm16,       {0x8F}, SIZE_W,     -1,     -1,     -1, -1, {0x66})
OPDEF(pop_rm64,       {0x8F}, SIZE_Q,     -1,     -1,     -1, -1, {})


OPDEF(ret,            {0xC3},     -1,     -1,     -1,     -1, -1, {})
OPDEF(ret_imm16,      {0xC2},     -1,     -1,     -1, SIZE_W, -1, {})


OPDEF(call_rel32,     {0xE8},     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(call_rm64,      {0xFF}, SIZE_Q,     -1,     -1,     -1,  2, {})


OPDEF(test_rm8_r8,    {0x84}, SIZE_B, SIZE_B,     -1,     -1, -1, {})
OPDEF(test_rm16_r16,  {0x85}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(test_rm32_r32,  {0x85}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(test_rm64_r64,  {0x85}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(test_rm8_imm8,  {0xF6}, SIZE_B,     -1,     -1, SIZE_B,  0, {})
OPDEF(test_rm16_imm16,{0xF7}, SIZE_W,     -1,     -1, SIZE_W,  0, {0x66})
OPDEF(test_rm32_imm32,{0xF7}, SIZE_D,     -1,     -1, SIZE_D,  0, {})
OPDEF(test_rm64_imm32,{0xF7}, SIZE_Q,     -1,     -1, SIZE_D,  0, {})


OPDEF(cmp_rm16_imm8,  {0x83}, SIZE_W,     -1,     -1, SIZE_B,  7, {0x66})
OPDEF(cmp_rm32_imm8,  {0x83}, SIZE_D,     -1,     -1, SIZE_B,  7, {})
OPDEF(cmp_rm64_imm8,  {0x83}, SIZE_Q,     -1,     -1, SIZE_B,  7, {})


OPDEF(jmp_rel8,       {0xEB},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jmp_rel32,      {0xE9},     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jmp_rm64,       {0xFF}, SIZE_Q,     -1,     -1,     -1,  4, {})


OPDEF(jo_rel8,        {0x70},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jno_rel8,       {0x71},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jc_rel8,        {0x72},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jb_rel8,        {0x72},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jae_rel8,       {0x73},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnb_rel8,       {0x73},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnc_rel8,       {0x73},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(je_rel8,        {0x74},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jz_rel8,        {0x74},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jne_rel8,       {0x75},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnz_rel8,       {0x75},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jbe_rel8,       {0x76},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jna_rel8,       {0x76},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(ja_rel8,        {0x77},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnbe_rel8,      {0x77},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(js_rel8,        {0x78},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jns_rel8,       {0x79},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jp_rel8,        {0x7A},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jpe_rel8,       {0x7A},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnp_rel8,       {0x7B},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jpo_rel8,       {0x7B},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jl_rel8,        {0x7C},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnge_rel8,      {0x7C},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jge_rel8,       {0x7D},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnl_rel8,       {0x7D},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jle_rel8,       {0x7E},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jng_rel8,       {0x7E},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jg_rel8,        {0x7F},     -1,     -1, SIZE_B,     -1, -1, {})
OPDEF(jnle_rel8,      {0x7F},     -1,     -1, SIZE_B,     -1, -1, {})

OPDEF(jo_rel32,       B_(0x0F, 0x80),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jno_rel32,      B_(0x0F, 0x81),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jc_rel32,       B_(0x0F, 0x82),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jb_rel32,       B_(0x0F, 0x82),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jae_rel32,      B_(0x0F, 0x83),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnb_rel32,      B_(0x0F, 0x83),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnc_rel32,      B_(0x0F, 0x83),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(je_rel32,       B_(0x0F, 0x84),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jz_rel32,       B_(0x0F, 0x84),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jne_rel32,      B_(0x0F, 0x85),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnz_rel32,      B_(0x0F, 0x85),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jbe_rel32,      B_(0x0F, 0x86),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jna_rel32,      B_(0x0F, 0x86),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(ja_rel32,       B_(0x0F, 0x87),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnbe_rel32,     B_(0x0F, 0x87),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(js_rel32,       B_(0x0F, 0x88),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jns_rel32,      B_(0x0F, 0x89),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jp_rel32,       B_(0x0F, 0x8A),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jpe_rel32,      B_(0x0F, 0x8A),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnp_rel32,      B_(0x0F, 0x8B),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jpo_rel32,      B_(0x0F, 0x8B),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jl_rel32,       B_(0x0F, 0x8C),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnge_rel32,     B_(0x0F, 0x8C),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jge_rel32,      B_(0x0F, 0x8D),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnl_rel32,      B_(0x0F, 0x8D),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jle_rel32,      B_(0x0F, 0x8E),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jng_rel32,      B_(0x0F, 0x8E),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jg_rel32,       B_(0x0F, 0x8F),     -1,     -1, SIZE_D,     -1, -1, {})
OPDEF(jnle_rel32,     B_(0x0F, 0x8F),     -1,     -1, SIZE_D,     -1, -1, {})

OPDEF(jecxz_rel8,     {0xE3},     -1,     -1, SIZE_B,     -1, -1, {0x67})
OPDEF(jrcxz_rel8,     {0xE3},     -1,     -1, SIZE_B,     -1, -1, {})


OPDEF(imul_r16_rm16,  B_(0x0F, 0xAF), SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(imul_r32_rm32,  B_(0x0F, 0xAF), SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(imul_r64_rm64,  B_(0x0F, 0xAF), SIZE_Q, SIZE_Q,     -1,     -1, -1, {})

OPDEF(imul_r16_rm16_imm8,     {0x6B}, SIZE_W, SIZE_W,     -1, SIZE_B, -1, {0x66})
OPDEF(imul_r32_rm32_imm8,     {0x6B}, SIZE_D, SIZE_D,     -1, SIZE_B, -1, {})
OPDEF(imul_r64_rm64_imm8,     {0x6B}, SIZE_Q, SIZE_Q,     -1, SIZE_B, -1, {})

OPDEF(imul_r16_rm16_imm16,    {0x69}, SIZE_W, SIZE_W,     -1, SIZE_W, -1, {0x66})
OPDEF(imul_r32_rm32_imm32,    {0x69}, SIZE_D, SIZE_D,     -1, SIZE_D, -1, {})
OPDEF(imul_r64_rm64_imm32,    {0x69}, SIZE_Q, SIZE_Q,     -1, SIZE_D, -1, {})


OPDEF(lea_r16_m,              {0x8D}, SIZE_W, SIZE_W,     -1,     -1, -1, {0x66})
OPDEF(lea_r32_m,              {0x8D}, SIZE_D, SIZE_D,     -1,     -1, -1, {})
OPDEF(lea_r64_m,              {0x8D}, SIZE_Q, SIZE_Q,     -1,     -1, -1, {})


OPDEF(nop,                    {0x90},     -1,     -1,     -1,     -1, -1, {})


OPDEF(seto_rm8,       B_(0x0F, 0x90), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setno_rm8,      B_(0x0F, 0x91), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setc_rm8,       B_(0x0F, 0x92), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setb_rm8,       B_(0x0F, 0x92), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setae_rm8,      B_(0x0F, 0x93), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnb_rm8,      B_(0x0F, 0x93), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnc_rm8,      B_(0x0F, 0x93), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(sete_rm8,       B_(0x0F, 0x94), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setz_rm8,       B_(0x0F, 0x94), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setne_rm8,      B_(0x0F, 0x95), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnz_rm8,      B_(0x0F, 0x95), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setbe_rm8,      B_(0x0F, 0x96), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setna_rm8,      B_(0x0F, 0x96), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(seta_rm8,       B_(0x0F, 0x97), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnbe_rm8,     B_(0x0F, 0x97), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(sets_rm8,       B_(0x0F, 0x98), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setns_rm8,      B_(0x0F, 0x99), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setp_rm8,       B_(0x0F, 0x9A), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setpe_rm8,      B_(0x0F, 0x9A), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnp_rm8,      B_(0x0F, 0x9B), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setpo_rm8,      B_(0x0F, 0x9B), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setl_rm8,       B_(0x0F, 0x9C), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnge_rm8,     B_(0x0F, 0x9C), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setge_rm8,      B_(0x0F, 0x9D), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnl_rm8,      B_(0x0F, 0x9D), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setle_rm8,      B_(0x0F, 0x9E), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setng_rm8,      B_(0x0F, 0x9E), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setg_rm8,       B_(0x0F, 0x9F), SIZE_B,     -1,     -1,     -1, -1, {})
OPDEF(setnle_rm8,     B_(0x0F, 0x9F), SIZE_B,     -1,     -1,     -1, -1, {})





/* ? rcl rcr rol ror shl shr sal sar */
/* some of the mmx operations for doubles (p___ and movq) */
/* converters */




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


OPDEF(mul_rm8,        {0xF6}, SIZE_B,     -1,     -1,     -1,  4, {})
OPDEF(mul_rm16,       {0xF7}, SIZE_W,     -1,     -1,     -1,  4, {0x66})
OPDEF(mul_rm32,       {0xF7}, SIZE_D,     -1,     -1,     -1,  4, {})
OPDEF(mul_rm64,       {0xF7}, SIZE_Q,     -1,     -1,     -1,  4, {})


OPDEF(imul_rm8,       {0xF6}, SIZE_B,     -1,     -1,     -1,  5, {})
OPDEF(imul_rm16,      {0xF7}, SIZE_W,     -1,     -1,     -1,  5, {0x66})
OPDEF(imul_rm32,      {0xF7}, SIZE_D,     -1,     -1,     -1,  5, {})
OPDEF(imul_rm64,      {0xF7}, SIZE_Q,     -1,     -1,     -1,  5, {})


OPDEF(div_rm8,        {0xF6}, SIZE_B,     -1,     -1,     -1,  6, {})
OPDEF(div_rm16,       {0xF7}, SIZE_W,     -1,     -1,     -1,  6, {0x66})
OPDEF(div_rm32,       {0xF7}, SIZE_D,     -1,     -1,     -1,  6, {})
OPDEF(div_rm64,       {0xF7}, SIZE_Q,     -1,     -1,     -1,  6, {})


OPDEF(idiv_rm8,       {0xF6}, SIZE_B,     -1,     -1,     -1,  7, {})
OPDEF(idiv_rm16,      {0xF7}, SIZE_W,     -1,     -1,     -1,  7, {0x66})
OPDEF(idiv_rm32,      {0xF7}, SIZE_D,     -1,     -1,     -1,  7, {})
OPDEF(idiv_rm64,      {0xF7}, SIZE_Q,     -1,     -1,     -1,  7, {})
*/



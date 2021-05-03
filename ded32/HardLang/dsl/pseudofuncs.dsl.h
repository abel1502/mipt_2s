/// DEF_PFUNC(RTYPECAP, RTYPEWORD, NAME, COMPILECODE)

/*DEF_PFUNC(VOID, Void, print_dbl, {
    PF_VERIFY_ARGCNT(1);
    PF_REQUIRE_CHILD_TYPE(0, Dbl);

    PF_COMPILE_CHILD(0);
    PF_O("out df:\n");
})

DEF_PFUNC(VOID, Void, print_int4, {
    PF_VERIFY_ARGCNT(1);
    PF_REQUIRE_CHILD_TYPE(0, Int4);

    PF_COMPILE_CHILD(0);
    PF_O("out dwl:\n");
})

DEF_PFUNC(VOID, Void, print_int8, {
    PF_VERIFY_ARGCNT(1);
    PF_REQUIRE_CHILD_TYPE(0, Int8);

    PF_COMPILE_CHILD(0);
    PF_O("out qw:\n");
})

DEF_PFUNC(DBL, Dbl, read_dbl, {
    PF_VERIFY_ARGCNT(0);

    PF_O("in df:\n");
})

DEF_PFUNC(INT4, Int4, read_int4, {
    PF_VERIFY_ARGCNT(0);

    PF_O("in dwl:\n");
})

DEF_PFUNC(INT8, Int8, read_int8, {
    PF_VERIFY_ARGCNT(0);

    PF_O("in qw:\n");
})*/

DEF_PFUNC(DBL, Dbl, sqrt, {
    PF_VERIFY_ARGCNT(1);

    PF_REQUIRE_CHILD_TYPE(0, Dbl);

    PF_COMPILE_CHILD(0);

    PF_PULL(1);

    PF_ADDINSTR();
    PF_LASTINSTR()
        .setOp(Opcode_e::movd_rx_rm64)
        .setR(REG_B)
        .setRmReg(PF_TOS(1));

    // PF_POP();

    PF_ADDINSTR();
    PF_LASTINSTR()
        .setOp(Opcode_e::sqrtsd_rx_rm64x)
        .setR(REG_A)
        .setRmReg(REG_B);

    // PF_PUSH();

    PF_ADDINSTR();
    PF_LASTINSTR()
        .setOp(Opcode_e::movd_rm64_rx)
        .setR(REG_A)
        .setRmReg(PF_TOS(1));
})

/*DEF_PFUNC(VOID, Void, meow, {
    PF_VERIFY_ARGCNT(1);

    PF_REQUIRE_CHILD_TYPE(0, Int4);

    PF_COMPILE_CHILD(0);
    PF_O("meow dwl:\n");
})*/

DEF_PFUNC(VOID, Void, brk, {
    PF_VERIFY_ARGCNT(0);

    PF_ADDINSTR();
    PF_LASTINSTR().setOp(Opcode_e::int3);
})

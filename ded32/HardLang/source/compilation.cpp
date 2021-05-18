#include "ast.h"


namespace abel {

bool Var::reference(Instruction &instr, const Scope *scope) const {
    return reference(instr, scope->getInfo(name));
}

bool Var::reference(Instruction &instr, VarInfo vi) {
    if (!vi.var) {
        //ERR("Unknown variable \"%.*s\"", name->getLength(), name->getStr());
        ERR("Shouldn't be reachable");
        assert(false);

        return true;
    }

    //TRY_B(vi.var->ts.compile(ofile));
    //fprintf(ofile, "[rz+%u]", vi.offset);

    // TODO: Maybe optimize to DISP_8?
    instr.setRmSib(REG_BP, Instruction::mode_t::DISP_32).setDisp(vi.offset);

    return false;
}

//================================================================================

bool Scope::exit(ObjectFactory &obj) {
    if (curOffset == 0)
        return false;

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::add_rm64_imm32)
        .setRmReg(REG_SP)
        .setImm(-curOffset);

    return false;
}

//================================================================================

bool Expression::compile(ObjectFactory &obj, Scope *scope, const Program *prog) {
    /*printf("; EXPR COMPILE\n");
    #define DEF_TYPE(NAME)                          \
        if (vtable_ == &Expression::VTYPE(NAME)) {  \
            printf("; ->" #NAME "\n");                          \
        } else
    #include "exprtypes.dsl.h"
    #undef DEF_TYPE
    {
        ERR("; ->UNKNOWN TYPE\n");
        assert(false);
    }*/
    return VCALL(this, compile, obj, scope, prog);
}

bool Expression::compileVarRecepient(ObjectFactory &obj, Scope *scope, const Program *) {
    assert(isVarRef());

    TRY_B(obj.stkPull(1));

    TRY_B(obj.addInstr());

    VarInfo vi = scope->getInfo(name);
    switch (vi.var->getType().type) {
    case TypeSpec::T_INT4:
        obj.getLastInstr().setOp(Opcode_e::mov_rm32_r32);
        break;

    case TypeSpec::T_INT8:
    case TypeSpec::T_DBL:  // XMM always holds a single double-precision value, and should in fact be placed in the regular register stack most of the time
        obj.getLastInstr().setOp(Opcode_e::mov_rm64_r64);
        break;

    case TypeSpec::T_VOID:
    NODEFAULT
    }

    TRY_B(Var::reference(obj.getLastInstr(), vi));

    obj.getLastInstr().setR(obj.stkTos(1));

    // TRY_B(obj.stkPop());  // Intentionally not called, as the value is supposed to be left on the stack

    return false;
}

bool Expression::compilePseudofunc(ObjectFactory &obj, Scope *scope, const Program *prog) {
    #define PF_DEDUCE_CHILD_TYPE(IND, MASK) \
        children[IND].deduceType(MASK, scope, prog)

    #define PF_REQUIRE_CHILD_TYPE(IND, TYPEWORD)                    \
        if (!PF_DEDUCE_CHILD_TYPE(IND, TypeSpec::TYPEWORD##Mask)) { \
            ERR("Argument #%u for %.*s must be %s", IND,            \
                name->getLength(), name->getStr(), #TYPEWORD);      \
                                                                    \
            return true;                                            \
        }

    #define PF_VERIFY_ARGCNT(CNT)                                   \
        if (children.getSize() != CNT) {                            \
            ERR("Pseudofunction %.*s takes exactly %u arguments",   \
                name->getLength(), name->getStr(), CNT);            \
                                                                    \
            return true;                                            \
        }

    #define PF_COMPILE_CHILD(IND) \
        TRY_B(children[IND].compile(obj, scope, prog));

    // TODO: Maybe encapsulate tos-push-pop, addInstr and getLastInstr

    #define PF_TOS(...)     obj.stkTos(__VA_ARGS__)
    #define PF_PUSH(...)    TRY_B(obj.stkPush(__VA_ARGS__))
    #define PF_POP(...)     TRY_B(obj.stkPop(__VA_ARGS__))
    #define PF_PULL(...)    TRY_B(obj.stkPull(__VA_ARGS__))
    #define PF_FLUSH(...)   TRY_B(obj.stkFlush(__VA_ARGS__))

    #define PF_ADDINSTR() \
        TRY_B(obj.addInstr())

    #define PF_LASTINSTR() \
        obj.getLastInstr()

    #define DEF_PFUNC(RTYPECAP, RTYPEWORD, NAME, COMPILECODE)               \
        if (sizeof(#NAME) == name->getLength() &&                           \
            strncmp(name->getStr() + 1, #NAME, sizeof(#NAME) - 1) == 0) {   \
                                                                            \
            COMPILECODE;                                                    \
            return false;                                                   \
        }
    #include "pseudofuncs.dsl.h"
    #undef DEF_PFUNC

    ERR("Unknown pseudo-function: \"%.*s\"", name->getLength(), name->getStr());

    return true;

    #undef PF_DEDUCE_CHILD_TYPE
    #undef PF_VERIFY_ARGCNT
    #undef PF_REQUIRE_CHILD_TYPE
    #undef PF_COMPILE_CHILD
    #undef PF_TOS
    #undef PF_PUSH
    #undef PF_POP
    #undef PF_PULL
    #undef PF_FLUSH
    #undef PF_ADDINSTR
    #undef PF_LASTINSTR
}

//--------------------------------------------------------------------------------

bool Expression::VMIN(Void, compile)(ObjectFactory &, Scope *, const Program *) {
    assert(children.getSize() == 0);

    // Let's leave it trivial for now, I guess
    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    TRY_B(exprType.type != TypeSpec::T_VOID);
    exprType.dtor();

    return false;
}

bool Expression::VMIN(Asgn, compile)(ObjectFactory &obj, Scope *scope, const Program *prog) {
    assert(children.getSize() == 2);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    TRY_B(exprType.type == TypeSpec::T_VOID);

    if (am != AM_EQ) {
        TRY_B(children[0].compile(obj, scope, prog));
    }

    TRY_B(children[1].compile(obj, scope, prog));

    if (am != AM_EQ) {
        TRY_B(obj.stkPull(2));

        switch (am) {
        case AM_ADDEQ:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::add_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::add_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::addsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case AM_SUBEQ:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::sub_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::sub_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::subsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case AM_MULEQ:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::imul_r32_rm32)
                    .setR(obj.stkTos(2))
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::imul_r64_rm64)
                    .setR(obj.stkTos(2))
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mulsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case AM_DIVEQ:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm32_r32)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm32)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_A);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm64_r64)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm64)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_A);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::divsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case AM_MODEQ:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm32_r32)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm32)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_D);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm64_r64)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm64)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_D);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                ERR("Remainder can't be computed for non-integral types");

                return true;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case AM_EQ:
        NODEFAULT
        }
    }

    TRY_B(children[0].compileVarRecepient(obj, scope, prog));

    exprType.dtor();

    return false;
}

bool Expression::VMIN(PolyOp, compile)(ObjectFactory &obj, Scope *scope, const Program *prog) {
    assert(children.getSize() > 0);
    //assert(children.getSize() > 1);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    TRY_B(exprType.type == TypeSpec::T_VOID);

    // For cmp the exprType is Int4, so we need the child's one
    if (typeMask != children[0].typeMask) {
        TRY_B(exprType.ctor(children[0].typeMask));
    }

    TRY_B(children[0].compile(obj, scope, prog));

    for (unsigned i = 0; i + 1 < children.getSize(); i++) {
        // We do rely on all the operators being left-to-right evaluated, so
        // take caution when adding new ones that don't follow this rule
        // (such as **, for example)

        TRY_B(children[i + 1].compile(obj, scope, prog));

        TRY_B(obj.stkPull(2));

        switch (ops[i]) {

        // TODO: Maybe simplify the comparisons (by somehow repositioning the stkPops)

        #define CMP_OP_(SETFLAG)                                \
            switch (exprType.type) {                            \
            case TypeSpec::T_INT4:                              \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::cmp_rm32_r32)              \
                    .setRmReg(obj.stkTos(2))                    \
                    .setR(obj.stkTos(1));                       \
                                                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::set##SETFLAG##_rm8)        \
                    .setRmReg(obj.stkTos(2));                   \
                                                                \
                /* TODO: This may be unnecessary, but it */     \
                /* compensates for the lack of || and && */     \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::and_rm64_imm32)            \
                    .setRmReg(obj.stkTos(2))                    \
                    .setImm(0xff);                              \
                                                                \
                TRY_B(obj.stkPop());                            \
                                                                \
                break;                                          \
                                                                \
            case TypeSpec::T_INT8:                              \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::cmp_rm64_r64)              \
                    .setRmReg(obj.stkTos(2))                    \
                    .setR(obj.stkTos(1));                       \
                                                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::set##SETFLAG##_rm8)        \
                    .setRmReg(obj.stkTos(2));                   \
                                                                \
                /* TODO: same */                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::and_rm64_imm32)            \
                    .setRmReg(obj.stkTos(2))                    \
                    .setImm(0xff);                              \
                                                                \
                TRY_B(obj.stkPop());                            \
                                                                \
                break;                                          \
                                                                \
            case TypeSpec::T_DBL:                               \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::movq_rx_rm64)              \
                    .setR(REG_A)                                \
                    .setRmReg(obj.stkTos(2));                   \
                                                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::movq_rx_rm64)              \
                    .setR(REG_B)                                \
                    .setRmReg(obj.stkTos(1));                   \
                                                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::ucomisd_rx_rm64x)          \
                    .setR(REG_A)                                \
                    .setRmReg(REG_B);                           \
                                                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::set##SETFLAG##_rm8)        \
                    .setRmReg(obj.stkTos(2));                   \
                                                                \
                /* TODO: same */                                \
                TRY_B(obj.addInstr());                          \
                obj.getLastInstr()                              \
                    .setOp(Opcode_e::and_rm64_imm32)            \
                    .setRmReg(obj.stkTos(2))                    \
                    .setImm(0xff);                              \
                                                                \
                TRY_B(obj.stkPop());                            \
                                                                \
                break;                                          \
                                                                \
            case TypeSpec::T_VOID:                              \
            NODEFAULT                                           \
            }

        case OP_EQ:
            CMP_OP_(e);

            break;

        case OP_NEQ:
            CMP_OP_(ne);

            break;

        case OP_GEQ:
            CMP_OP_(ae);

            break;

        case OP_LEQ:
            CMP_OP_(be);

            break;

        case OP_LT:
            CMP_OP_(b);

            break;

        case OP_GT:
            CMP_OP_(a);

            break;

        #undef CMP_OP_

        case OP_ADD:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::add_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::add_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::addsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case OP_SUB:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::sub_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::sub_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::subsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case OP_MUL:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::imul_r32_rm32)
                    .setR(obj.stkTos(2))
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::imul_r64_rm64)
                    .setR(obj.stkTos(2))
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mulsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case OP_DIV:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm32_r32)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm32)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_A);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm64_r64)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm64)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_A);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rx_rm64)
                    .setR(REG_B)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::divsd_rx_rm64x)
                    .setR(REG_A)
                    .setRmReg(REG_B);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx)
                    .setR(REG_A)
                    .setRmReg(obj.stkTos(2));

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        case OP_MOD:
            switch (exprType.type) {
            case TypeSpec::T_INT4:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm32_r32)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm32)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm32_r32)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_D);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_INT8:
                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::xor_rm64_r64)
                    .setRmReg(REG_D)
                    .setR(REG_D);

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(REG_A)
                    .setR(obj.stkTos(2));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::idiv_rm64)
                    .setRmReg(obj.stkTos(1));

                TRY_B(obj.addInstr());
                obj.getLastInstr()
                    .setOp(Opcode_e::mov_rm64_r64)
                    .setRmReg(obj.stkTos(2))
                    .setR(REG_D);

                TRY_B(obj.stkPop());

                break;

            case TypeSpec::T_DBL:
                ERR("Remainder can't be computed for non-integral types");

                return true;

            case TypeSpec::T_VOID:
            NODEFAULT
            }

            break;

        NODEFAULT
        }

        // TODO: Eventually should change poly-comparison handling to the pythonic way.
        // TODO: For now, in fact, I should probably forbid a == b == c stuff at all
    }

    exprType.dtor();

    return false;
}

bool Expression::VMIN(Neg, compile)(ObjectFactory &obj, Scope *scope, const Program *prog) {
    assert(children.getSize() == 1);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    TRY_B(exprType.type == TypeSpec::T_VOID);

    TRY_B(children[0].compile(obj, scope, prog));

    TRY_B(obj.stkPull(1));

    switch (exprType.type) {
    case TypeSpec::T_INT4:
        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::neg_rm32)
            .setRmReg(obj.stkTos(1));

        break;

    case TypeSpec::T_INT8:
        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::neg_rm64)
            .setRmReg(obj.stkTos(1));

        break;

    case TypeSpec::T_DBL:
        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::xorpd_rx_rm128x)
            .setR(REG_A)
            .setRmReg(REG_A);

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::movq_rx_rm64)
            .setR(REG_B)
            .setRmReg(obj.stkTos(1));

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::subsd_rx_rm64x)
            .setR(REG_A)
            .setRmReg(REG_B);

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::movq_rm64_rx)
            .setR(REG_A)
            .setRmReg(obj.stkTos(1));

        break;

    case TypeSpec::T_VOID:
    NODEFAULT
    }

    exprType.dtor();

    return false;
}

bool Expression::VMIN(Cast, compile)(ObjectFactory &obj, Scope *scope, const Program *prog) {
    assert(children.getSize() == 1);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));

    TypeSpec childType{};
    TypeSpec::Mask tmpMask = children[0].deduceType(TypeSpec::AllMask, scope, prog);

    if (tmpMask == (TypeSpec::Int4Mask | TypeSpec::Int8Mask)) {
        // A workaround for dbl:123 and such
        tmpMask = children[0].deduceType(TypeSpec::Int4Mask, scope, prog);
    }

    if (childType.ctor(tmpMask)){
        ERR("Ambiguous type");
        return true;
    }

    TRY_B(children[0].compile(obj, scope, prog));

    if (childType.type != TypeSpec::T_VOID)
        TRY_B(obj.stkPull(1));

    switch (exprType.type) {
    case TypeSpec::T_VOID:
        ERR("Can't cast to void");
        return true;

    case TypeSpec::T_DBL:
        switch (childType.type) {
        case TypeSpec::T_VOID:
            TRY_B(obj.stkPush());

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::xor_rm64_r64)
                .setRmReg(obj.stkTos(1))
                .setR(obj.stkTos(1));

            break;

        case TypeSpec::T_DBL:
            break;

        case TypeSpec::T_INT4:
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::cvtsi2sd_rx_rm32)
                .setR(REG_A)
                .setRmReg(obj.stkTos(1));

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rm64_rx)
                .setRmReg(obj.stkTos(1))
                .setR(REG_A);

            break;

        case TypeSpec::T_INT8:
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::cvtsi2sd_rx_rm64)
                .setR(REG_A)
                .setRmReg(obj.stkTos(1));

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rm64_rx)
                .setRmReg(obj.stkTos(1))
                .setR(REG_A);

            break;

        NODEFAULT
        }
        break;

    case TypeSpec::T_INT8:
        switch (childType.type) {
        case TypeSpec::T_VOID:
            TRY_B(obj.stkPush());

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::xor_rm64_r64)
                .setRmReg(obj.stkTos(1))
                .setR(obj.stkTos(1));

            break;

        case TypeSpec::T_DBL:
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rx_rm64)
                .setR(REG_A)
                .setRmReg(obj.stkTos(1));

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::cvtsd2si_r64_rm64x)
                .setR(obj.stkTos(1))
                .setRmReg(REG_A);

            break;

        case TypeSpec::T_INT4:
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::movsx_r64_rm32)
                .setR(obj.stkTos(1))
                .setRmReg(obj.stkTos(1));

            break;

        case TypeSpec::T_INT8:
            break;

        NODEFAULT
        }
        break;

    case TypeSpec::T_INT4:
        switch (childType.type) {
        case TypeSpec::T_VOID:
            TRY_B(obj.stkPush());

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::xor_rm32_r32)
                .setRmReg(obj.stkTos(1))
                .setR(obj.stkTos(1));

            break;

        case TypeSpec::T_DBL:
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rx_rm64)
                .setR(REG_A)
                .setRmReg(obj.stkTos(1));

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::cvtsd2si_r32_rm64x)
                .setR(obj.stkTos(1))
                .setRmReg(REG_A);

            break;

        case TypeSpec::T_INT4:
            break;

        case TypeSpec::T_INT8:
            // TODO: Maybe unnecessary?

            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::mov_rm32_r32)
                .setRmReg(obj.stkTos(1))
                .setR(obj.stkTos(1));

            break;

        NODEFAULT
        }
        break;

    NODEFAULT
    }

    childType.dtor();
    exprType.dtor();

    return false;
}

bool Expression::VMIN(Num, compile)(ObjectFactory &obj, Scope *, const Program *) {
    assert(children.getSize() == 0);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    //TRY_B(exprType.type == TypeSpec::T_VOID);

    double immVal = 0.;
    static_assert(sizeof(immVal) == 8);

    switch (exprType.type) {
    case TypeSpec::T_VOID:
        ERR("Void isn't a number");
        return true;

    case TypeSpec::T_INT4:
        TRY_B(obj.stkPush());

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::mov_r32_imm32)
            .setR(obj.stkTos(1))
            .setImm(num->asInt());

        break;

    case TypeSpec::T_INT8:
        TRY_B(obj.stkPush());

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::mov_r64_imm64)
            .setR(obj.stkTos(1))
            .setImm(num->asInt());

        break;

    case TypeSpec::T_DBL:
        TRY_B(obj.stkPush());

        immVal = num->asDbl();

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::mov_r64_imm64)
            .setR(obj.stkTos(1))
            .setImm(*(uint64_t *)&immVal);  // TODO: Test correctness

        break;

    NODEFAULT
    }

    exprType.dtor();

    return false;
}

bool Expression::VMIN(VarRef, compile)(ObjectFactory &obj, Scope *scope, const Program *) {
    assert(children.getSize() == 0);

    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));
    TRY_B(exprType.type == TypeSpec::T_VOID);

    VarInfo vi = scope->getInfo(name);

    assert(vi.var);  // If it didn't exist, type deduction would have already failed
    /*if (!vi.var) {
        ERR("Unknown variable: \"%.*s\"", name->getLength(), name->getStr());
        return true;
    }*/

    TRY_B(obj.stkPush());

    TRY_B(obj.addInstr());

    switch (vi.var->getSize()) {
    case SIZE_B:
        obj.getLastInstr().setOp(Opcode_e::mov_r8_rm8);
        break;

    case SIZE_W:
        obj.getLastInstr().setOp(Opcode_e::mov_r16_rm16);
        break;

    case SIZE_D:
        obj.getLastInstr().setOp(Opcode_e::mov_r32_rm32);
        break;

    case SIZE_Q:
    case SIZE_XMM:
        obj.getLastInstr().setOp(Opcode_e::mov_r64_rm64);
        break;

    NODEFAULT
    }

    TRY_B(Var::reference(obj.getLastInstr(), vi));

    obj.getLastInstr().setR(obj.stkTos(1));

    exprType.dtor();

    return false;
}

bool Expression::VMIN(FuncCall, compile)(ObjectFactory &obj, Scope *scope, const Program *prog) {
    TypeSpec exprType{};
    TRY_BC(exprType.ctor(typeMask), ERR("Ambiguous type"));

    if (name->getLength() >= 1 && name->getStr()[0] == '_') {
        return compilePseudofunc(obj, scope, prog);
    }

    const Function *func = prog->getFunction(name);

    TRY_BC(!func, ERR("Unknown function: \"%.*s\"", name->getLength(), name->getStr()));

    assert(func->getRtype().type == exprType.type);  // Should have been guaranteed by the deduceType

    const Vector<Var> *args = func->getArgs();

    // TODO: Maybe adjust word suffixes according to the actual numbers
    if (children.getSize() < args->getSize()) {
        ERR("Function \"%.*s\" takes %u arguments, but only %u were given",
            name->getLength(), name->getStr(), args->getSize(), children.getSize());

        return true;
    }

    if (children.getSize() > args->getSize()) {
        ERR("Function \"%.*s\" passed %u arguments, but takes only %u",
            name->getLength(), name->getStr(), children.getSize(), args->getSize());

        return true;
    }

    TRY_B(obj.stkFlush());

    unsigned stkNDelta = std::max(4u, args->getSize());

    TRY_B(obj.stkAlign(scope->getFrameSize(), stkNDelta));

    if (stkNDelta > args->getSize()) {
        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::sub_rm64_imm8)
            .setRmReg(REG_SP)
            .setImm(8 * (stkNDelta - args->getSize()));

        obj.stkPushMem(stkNDelta - args->getSize());
    }

    for (unsigned i = args->getSize() - 1; i != -1u; --i) {
        TypeSpec::Mask childMask = children[i].deduceType((*args)[i].getType().getMask(), scope, prog);

        if (!childMask) {
            ERR("Bad argument type (line %u, col %u: function \"%.*s\", argument %u)",
                name->getLine() + 1, name->getCol() + 1, name->getLength(), name->getStr(), i + 1);
            return true;
        }

        if (!TypeSpec::isMaskUnambiguous(childMask)) {  // TODO: Review usages of this function and maybe remove unnecessary zero checks
            ERR("Ambiguous argument type (line %u, col %u: function \"%.*s\", argument %u)",
                name->getLine() + 1, name->getCol() + 1, name->getLength(), name->getStr(), i + 1);
            return true;
        }

        assert(childMask != TypeSpec::VoidMask);

        children[i].compile(obj, scope, prog);
    }

    TRY_B(obj.stkFlush());

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::call_rel32)
        .setDisp(0);  // SYMBOL (name)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorFunction(name));

    assert(obj.stkRegIsEmpty());  // The object should believe this until now

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::add_rm64_imm8)
        .setRmReg(REG_SP)
        .setImm(8 * stkNDelta);

    obj.stkPopMem(stkNDelta);

    TRY_B(obj.stkUnalign());

    if (func->getRtype().type != TypeSpec::T_VOID) {
        TRY_B(obj.stkPush());

        if (func->getType() == Function::T_C_IMPORT) {
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::mov_rm64_r64)
                .setRmReg(obj.stkTos())
                .setR(REG_A);

            if (func->getRtype().type == TypeSpec::T_DBL) {
                obj.getLastInstr()
                    .setOp(Opcode_e::movq_rm64_rx);
            }
        }
    }

    exprType.dtor();

    return false;
}

//================================================================================

// TODO: Add some asserts for memory stack balance
bool Code::compile(ObjectFactory &obj, TypeSpec rtype, const Program *prog, const Function *func) {
    assert(obj.stkRegIsEmpty());

    for (unsigned i = 0; i < stmts.getSize(); ++i) {
        TRY_B(stmts[i].compile(obj, &scope, rtype, prog, func));

        assert(obj.stkRegIsEmpty());
    }

    return false;
}

//================================================================================

bool Statement::compile(ObjectFactory &obj, Scope *scope, TypeSpec rtype, const Program *prog, const Function *func) {
    return VCALL(this, compile, obj, scope, rtype, prog, func);
}

//--------------------------------------------------------------------------------

bool Statement::VMIN(Compound, compile)(ObjectFactory &obj, Scope *scope, TypeSpec rtype, const Program *prog, const Function *func) {
    code.getScope()->setParent(scope);
    TRY_B(code.compile(obj, rtype, prog, func));

    TRY_B(code.getScope()->exit(obj));

    return false;
}

bool Statement::VMIN(Return, compile)(ObjectFactory &obj, Scope *scope, TypeSpec rtype, const Program *prog, const Function *func) {
    if (rtype.type == TypeSpec::T_VOID) {
        if (!expr.isVoid()) {
            ERR("Void functions mustn't return values");

            return true;
        }  // TODO: Maybe also compile void expression, but make it trivial?

        // TRY_B(obj.stkFlush());
    } else {
        expr.deduceType(rtype.getMask(), scope, prog);

        TRY_B(expr.compile(obj, scope, prog));

        TRY_B(obj.stkFlushExceptOne());
    }

    // TODO: Add to rsp to clear the frame... ? Or not?

    /*TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jmp_rel32)
        .setDisp(0);  // Symbol (return from function)
    TRY_B(obj.getLastInstr().getSymbolHere()->ctorLabel());*/

    func->compileRet(obj);

    if (rtype.type != TypeSpec::T_VOID) {  // TODO: Unnecessary?
        TRY_B(obj.stkPop());
    }

    return false;
}

bool Statement::VMIN(Loop, compile)(ObjectFactory &obj, Scope *scope, TypeSpec rtype, const Program *prog, const Function *func) {
    unsigned loopLbl = obj.placeLabel();
    TRY_B(loopLbl == -1u);

    unsigned endLbl = obj.reserveLabel();

    TRY_BC(expr.deduceType(TypeSpec::AllMask & ~TypeSpec::VoidMask, scope, prog) != TypeSpec::Int4Mask,
           ERR("Ambiguous type"));

    TRY_B(expr.compile(obj, scope, prog));

    TRY_B(obj.stkPull(1));
    TRY_B(obj.stkPop());  // TODO: Check. This shouldn't overwrite the old value, but you never know

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::test_rm32_r32)
        .setRmReg(obj.stkTos(0))
        .setR(obj.stkTos(0));

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jz_rel32)
        .setDisp(0);  // SYMBOL!!! (endLbl)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorLabel(endLbl));

    code.getScope()->setParent(scope);  // TODO: Maybe get rid of all of those and fix this in the parser
    TRY_B(code.compile(obj, rtype, prog, func));

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jmp_rel32)
        .setDisp(0);  // SYMBOL!!! (loopLbl)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorLabel(loopLbl));

    TRY_B(obj.placeLabel(endLbl));

    return false;
}

bool Statement::VMIN(Cond, compile)(ObjectFactory &obj, Scope *scope, TypeSpec rtype, const Program *prog, const Function *func) {
    // TODO: Maybe simplify the empty else

    TRY_BC(expr.deduceType(TypeSpec::AllMask & ~TypeSpec::VoidMask, scope, prog) != TypeSpec::Int4Mask,
           ERR("Ambiguous type"));

    TRY_B(expr.compile(obj, scope, prog));

    unsigned elseLbl = obj.reserveLabel();
    unsigned  endLbl = obj.reserveLabel();

    TRY_B(obj.stkPull(1));
    TRY_B(obj.stkPop());  // TODO: Check

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::test_rm32_r32)
        .setRmReg(obj.stkTos(0))
        .setR(obj.stkTos(0));

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jz_rel32)
        .setDisp(0);  // SYMBOL!!! (elseLbl)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorLabel(elseLbl));

    code.getScope()->setParent(scope);
    TRY_B(code.compile(obj, rtype, prog, func));

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jmp_rel32)
        .setDisp(0);  // SYMBOL!!! (endLbl)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorLabel(endLbl));

    TRY_B(obj.placeLabel(elseLbl));

    altCode.getScope()->setParent(scope);
    TRY_B(altCode.compile(obj, rtype, prog, func));

    TRY_B(obj.placeLabel(endLbl));

    return false;
}

bool Statement::VMIN(VarDecl, compile)(ObjectFactory &obj, Scope *scope, TypeSpec, const Program *prog, const Function *) {
    TRY_B(scope->addVar(&var));

    assert(obj.stkRegIsEmpty());  // And so should be the memory part, in fact

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::sub_rm64_imm8)
        .setRmReg(REG_SP)
        .setImm(8);

    TypeSpec::Mask mask = expr.deduceType(TypeSpec::VoidMask | var.getType().getMask(), scope, prog);

    TRY_B(expr.compile(obj, scope, prog));  // TODO: Expr again

    if (mask != TypeSpec::VoidMask) {
        TRY_B(obj.stkPull(1));

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::mov_rm64_r64)
            .setR(obj.stkTos(1));

        TRY_B(var.reference(obj.getLastInstr(), scope));

        TRY_B(obj.stkPop());
    }

    return false;
}

bool Statement::VMIN(Expr, compile)(ObjectFactory &obj, Scope *scope, TypeSpec, const Program *prog, const Function *) {
    TypeSpec::Mask mask = expr.deduceType(TypeSpec::AllMask, scope, prog);

    TRY_B(expr.compile(obj, scope, prog));  // Ambiguousness of the mask is checked inside

    if (mask != TypeSpec::VoidMask) {
        TRY_B(obj.stkPop());
    }

    return false;
}

bool Statement::VMIN(Empty, compile)(ObjectFactory &, Scope *, TypeSpec, const Program *, const Function *) {
    return false;
}

//================================================================================

constexpr unsigned CARGS_REGS_SIZE = 4;
constexpr reg_e CARGS_REGS[CARGS_REGS_SIZE] = {REG_C, REG_D, REG_8, REG_9};
constexpr reg_e CARGS_REGS_XMM[CARGS_REGS_SIZE] = {(reg_e)0, (reg_e)1, (reg_e)2, (reg_e)3};

constexpr unsigned CSAVED_SPACE = 64;
constexpr unsigned CSAVED_REGS_SIZE = 7;
constexpr reg_e CSAVED_REGS[CSAVED_REGS_SIZE] = {REG_12, REG_13, REG_14, REG_15, REG_DI, REG_SI, REG_B};

bool Function::compileBody(ObjectFactory &obj, const Program *prog) {
    obj.stkReset();

    code.getScope()->setParent(nullptr);

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::push_rm64)
        .setRmReg(REG_BP);

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::mov_rm64_r64)
        .setRmReg(REG_BP)
        .setR(REG_SP);

    if (type == T_C_EXPORT) {
        for (unsigned i = 0; i < CSAVED_REGS_SIZE; ++i) {
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::mov_rm64_r64)
                .setRmSib(REG_SP, Instruction::mode_t::DISP_8)
                .setDisp(-8 * (i + 1))
                .setR(CSAVED_REGS[i]);
        }

        TRY_B(obj.addInstr());
        obj.getLastInstr()
            .setOp(Opcode_e::sub_rm64_imm8)
            .setRmReg(REG_SP)
            .setImm(CSAVED_SPACE);
    }

    TRY_B(code.compile(obj, rtype, prog, this));

    compileRet(obj);  // Equivalent to either returning void or a garbage value,
                      // DOESN'T break the stack or anything

    return false;
}

bool Function::compileCCaller(ObjectFactory &obj, const Program *) {
    /*

    changeArgs (to registers)
    jmp

    */

    obj.stkReset();

    for (unsigned i = 0; i < CARGS_REGS_SIZE && i < args.getSize(); ++i) {
        TRY_B(obj.addInstr());

        if (args[i].getType().type == TypeSpec::T_DBL) {
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rx_rm64)
                .setR(CARGS_REGS_XMM[i]);
        } else {
            obj.getLastInstr()
                .setOp(Opcode_e::mov_r64_rm64)
                .setR(CARGS_REGS[i]);
        }

        obj.getLastInstr()
            .setRmSib(REG_SP, Instruction::mode_t::DISP_8)
            .setDisp(8 * (i + 1));
    }

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jmp_rel32)
        .setDisp(0);  // SYMBOL (name)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorFunction(name));

    return false;
}

bool Function::compileCCallee(ObjectFactory &obj, const Program *) {
    /*

    changeArgs (from regs to padded space)
    jmp

    */

    obj.stkReset();

    for (unsigned i = 0; i < CARGS_REGS_SIZE && i < args.getSize(); ++i) {
        TRY_B(obj.addInstr());

        if (args[i].getType().type == TypeSpec::T_DBL) {
            obj.getLastInstr()
                .setOp(Opcode_e::movq_rm64_rx)
                .setR(CARGS_REGS_XMM[i]);
        } else {
            obj.getLastInstr()
                .setOp(Opcode_e::mov_rm64_r64)
                .setR(CARGS_REGS[i]);
        }

        obj.getLastInstr()
            .setRmSib(REG_SP, Instruction::mode_t::DISP_8)
            .setDisp(8 * (i + 1));
    }

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::jmp_rel32)
        .setDisp(0);  // SYMBOL (name)
    TRY_B(obj.getLastInstr().getDispSymbol()->ctorFunction(name->getStr(), name->getLength()));

    return false;
}

bool Function::compile(ObjectFactory &obj, const Program *prog) {
    assert(prog);

    switch (type) {
    case T_DEF: {
        if (hasCode) {
            TRY_B(obj.defineFunction(name, true, true));  // Mangled, exported

            TRY_B(compileBody(obj, prog));
        } else {
            TRY_B(obj.importFunction(name, true));  // Mangled, imported
        }
    } break;

    case T_STATIC: {
        TRY_BC(!hasCode, ERR("Static functions must have code."));

        TRY_B(obj.defineFunction(name, true, false));  // Mangled, not exported

        TRY_B(compileBody(obj, prog));
    } break;

    case T_C_EXPORT: {
        TRY_B(obj.defineFunction(name, false, true));  // Unmangled C-variant is exported

        TRY_B(compileCCallee(obj, prog));

        if (hasCode) {
            TRY_B(obj.defineFunction(name, true, true));  // Mangled non-C variant, also exported

            TRY_B(compileBody(obj, prog));
        } else {
            TRY_B(obj.importFunction(name, true));  // Mangled, imported
        }
    } break;

    case T_C_IMPORT: {
        TRY_BC(hasCode, ERR("Import functions mustn't have code."));

        TRY_B(obj.importFunction(name, false));  // Unmangled C-variant, impoted

        TRY_B(obj.defineFunction(name, false, true));  // Mangled wrapped, exported

        TRY_B(compileCCaller(obj, prog));
    } break;

    NODEFAULT
    }

    return false;
}

bool Function::compileRet(ObjectFactory &obj) const {
    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::mov_rm64_r64)
        .setRmReg(REG_SP)
        .setR(REG_BP);

    if (type == T_C_EXPORT) {
        for (unsigned i = 0; i < CSAVED_REGS_SIZE; ++i) {
            TRY_B(obj.addInstr());
            obj.getLastInstr()
                .setOp(Opcode_e::mov_r64_rm64)
                .setRmSib(REG_BP, Instruction::mode_t::DISP_8)
                .setDisp(-8 * (i + 1))
                .setR(CSAVED_REGS[i]);
        }
    }

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::pop_rm64)
        .setRmReg(REG_BP);

    TRY_B(obj.addInstr());
    obj.getLastInstr()
        .setOp(Opcode_e::ret);

    return false;
}

//================================================================================

bool Program::compile(ObjectFactory &obj) {
    // TODO: Entrypoint will be in the standard library

    bool seenMain = false;  // TODO: Unnecessary with external linkage

    for (unsigned i = 0; i < functions.getSize(); ++i) {
         TRY_B(functions[i].compile(obj, this));

         seenMain |= functions[i].isMain();
     }

    if (seenMain) {
        ERR("Warning: No main function present. (If it exists, make sure it doesn't return anything)");
    }

    return false;
}

}

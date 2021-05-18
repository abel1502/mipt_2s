#include "ast.h"


namespace abel {

bool TypeSpec::ctor() {
    type = T_VOID;

    return false;
}

bool TypeSpec::ctor(Type_e new_type) {
    type = new_type;

    return false;
}

bool TypeSpec::ctor(Mask mask) {
    //printf("DEBUG: 0x%x, %s unambiguous\n", mask, isMaskUnambiguous(mask) ? "is" : "isn't");

    if (!isMaskUnambiguous(mask))
        return true;

    type = (Type_e)__builtin_ctz(mask);  // Almost txlib level magic)

    assert(type < TYPES_COUNT);

    return false;
}

void TypeSpec::dtor() {
    type = T_VOID;
}

void TypeSpec::reconstruct(FILE *ofile) const {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

    switch (type) {
    case T_DBL:
        fprintf(ofile, "dbl:");
        break;

    case T_INT4:
        fprintf(ofile, "int4:");
        break;

    case T_INT8:
        fprintf(ofile, "int8:");
        break;

    case T_VOID:
        fprintf(ofile, "???:");
        ERR("Void is not a valid type");
        // FALLTHROUGH
    NODEFAULT
    }

    #pragma GCC diagnostic pop
}

uint32_t TypeSpec::getSize() const {
    switch (type) {
    case T_DBL:
        return 8;

    case T_INT4:
        return 4;

    case T_INT8:
        return 8;

    case T_VOID:
        return 0;  // TODO: Fail?

    NODEFAULT
    }
}

TypeSpec::Mask TypeSpec::getMask() const {
    return 1 << type;
}

bool TypeSpec::fitsMask(Mask mask) const {
    return getMask() & mask;
}

//================================================================================

// Reserved for VarInfo

//================================================================================

bool Var::ctor() {
    ts = {};
    name = nullptr;

    return false;
}

bool Var::ctor(TypeSpec new_ts, const Token* new_name) {
    ts = new_ts;
    name = new_name;

    return false;
}

void Var::dtor() {
    ts = {};
    name = nullptr;
}

const TypeSpec Var::getType() const {
    return ts;
}

size_e Var::getSize() const {
    switch (ts.type) {
    case ts.T_INT4:
        return SIZE_D;

    case ts.T_INT8:
        return SIZE_Q;

    case ts.T_DBL:
        return SIZE_XMM;

    case ts.T_VOID:
    NODEFAULT
    }
}

const Token *Var::getName() const {
    return name;
}

//================================================================================

bool Scope::ctor() {
    TRY_B(vars.ctor());
    curOffset = 0;
    savedSpace = 0;
    parent = nullptr;

    return false;
}

void Scope::dtor() {
    vars.dtor();
    parent = nullptr;
    curOffset = 0;
    savedSpace = 0;
}

VarInfo Scope::getInfo(const Var *var) const {
    return getInfo(var->getName());
}

VarInfo Scope::getInfo(const Token *name) const {
    const Scope *cur = this;

    while (cur && !cur->vars.has(name->getStr(), name->getLength())) {
        cur = cur->parent;
    }

    if (!cur)
        return {0, nullptr};

    VarInfo vi = cur->vars.get(name->getStr(), name->getLength());
    if (vi.offset < 0)
        vi.offset -= savedSpace;

    return vi;
}

bool Scope::hasVar(const Var *var) const {
    return hasVar(var->getName());
}

bool Scope::hasVar(const Token *name) const {
    const Scope *cur = this;

    while (cur && !cur->vars.has(name->getStr(), name->getLength())) {
        cur = cur->parent;
    }

    return cur;
}

bool Scope::addVar(const Var *var) {
    if (vars.has(var->getName()->getStr(), var->getName()->getLength())) {
        ERR("Redeclaration of variable \"%.*s\" within the same scope", var->getName()->getLength(), var->getName()->getStr());

        return true;
    }

    //curOffset -= var->getType().getSize();
    assert(var->getType().getSize() <= 8);
    curOffset -= 8;

    TRY_B(vars.set(var->getName()->getStr(), var->getName()->getLength(), {curOffset, var}));

    return false;
}

bool Scope::addArg(const Var *arg, unsigned idx) {
    if (vars.has(arg->getName()->getStr(), arg->getName()->getLength())) {
        ERR("Redeclaration of argument \"%.*s\" within the same scope", arg->getName()->getLength(), arg->getName()->getStr());

        return true;
    }

    TRY_B(vars.set(arg->getName()->getStr(), arg->getName()->getLength(), {(int32_t)(8 * (idx + 2)), arg}));  // TODO: Check

    return false;
}

void Scope::setParent(const Scope *new_parent) {
    //printf("New parent: %p -> %p\n", parent, new_parent);

    //assert(parent == nullptr || parent == new_parent || new_parent == nullptr);

    parent = new_parent;

    if (parent) {
        assert(curOffset == 0);

        curOffset = parent->curOffset;
        savedSpace = parent->savedSpace;
    }
}

uint32_t Scope::getFrameSize() const {
    return -curOffset;
    /*uint32_t result = 0;
    const Scope *cur = this;

    while (cur) {
        uint32_t curSize = -(cur->curOffset);
        result += curSize;

        cur = cur->parent;
    }

    return result;*/
}

//================================================================================

bool Expression::ctor() {
    VINIT();

    TRY_B(children.ctor());
    typeMask = TypeSpec::AllMask;

    // TODO: zero-fill?
    return false;
}

bool Expression::ctorVoid() {
    TRY_B(ctor());

    VSETTYPE(this, Void);

    typeMask = TypeSpec::VoidMask;

    return false;
}

bool Expression::ctorAsgn() {
    TRY_B(ctor());

    VSETTYPE(this, Asgn);

    am = Expression::AM_EQ;
    TRY_B(children.ctor());

    return false;
}

bool Expression::ctorPolyOp() {
    TRY_B(ctor());

    VSETTYPE(this, PolyOp);

    TRY_B(ops.ctor());
    TRY_B(children.ctor());

    return false;
}

bool Expression::ctorNeg() {
    TRY_B(ctor());

    VSETTYPE(this, Neg);

    TRY_B(children.ctor());

    return false;
}

bool Expression::ctorCast(TypeSpec ts) {
    TRY_B(ctor());

    VSETTYPE(this, Cast);

    typeMask = ts.getMask();
    cast = ts;
    TRY_B(children.ctor());

    return false;
}

bool Expression::ctorNum(const Token *new_num) {
    TRY_B(ctor());

    VSETTYPE(this, Num);

    assert(new_num->isNum());
    num = new_num;

    if (num->isInteger()) {
        typeMask = TypeSpec::Int4Mask | TypeSpec::Int8Mask;
    } else {
        typeMask = TypeSpec::DblMask;
    }

    return false;
}

bool Expression::ctorVarRef(const Token *new_name) {
    TRY_B(ctor());

    VSETTYPE(this, VarRef);

    assert(new_name->isName());
    name = new_name;

    return false;
}

bool Expression::ctorFuncCall(const Token* new_name) {
    TRY_B(ctor());

    VSETTYPE(this, FuncCall);

    assert(new_name->isName());
    name = new_name;
    TRY_B(children.ctor());

    return false;
}

void Expression::dtor() {
    children.dtor();
    typeMask = TypeSpec::NoneMask;

    if (vtable_)
        VCALL(this, dtor);
}

bool Expression::makeChild(Expression** child) {
    TRY_B(children.append({}));

    *child = &children[-1];

    return false;
}

void Expression::popChild() {
    children.pop();
}

void Expression::setAsgnMode(AsgnMode_e mode) {
    assert(isAsgn());

    am = mode;
}

bool Expression::setOp(unsigned ind, Op_e op) {
    assert(isPolyOp());

    if (ind == ops.getSize()) {
        TRY_B(ops.append(op));
    }

    assert(ind < ops.getSize());
    ops[ind] = op;

    return false;
}

void Expression::simplifySingleChild() {  // TODO: Maybe rewrite with C++ features...
    if (!isPolyOp() || children.getSize() != 1)
        return;

    Expression singleChild{};
    memcpy(&singleChild, &children[0], sizeof(Expression));

    children.pop();  // Destructor intentionally not called

    dtor();

    memcpy(this, &singleChild, sizeof(Expression));

    // memset(&singleChild, 0, sizeof(Expression));  // Would have done to avoid implicit destruction, but
                                                     // thanks the C coding style we don't have it at all

    // And now the singleChild get silently removed, with no destructors called...
}

TypeSpec::Mask Expression::deduceType(TypeSpec::Mask mask, Scope *scope, const Program *prog) {
    if (!typeMask || TypeSpec::isMaskUnambiguous(typeMask)) {  // TODO: Maybe a different condition
        return typeMask;
    }

    typeMask &= mask;
    typeMask &= VCALL(this, deduceType, scope, prog);

    return typeMask;
}

void Expression::reconstruct(FILE *ofile) const {
    fprintf(ofile, "(");
    VCALL(this, reconstruct, ofile);
    fprintf(ofile, ")");
}

TypeSpec::Mask Expression::getPseudofuncRtypeMask() const {
    // NAME doesn't include the leading '_', which causes slight changes in string-name comparison;
    #define DEF_PFUNC(RTYPECAP, RTYPEWORD, NAME, COMPILECODE)               \
        if (sizeof(#NAME) == name->getLength() &&                           \
            strncmp(name->getStr() + 1, #NAME, sizeof(#NAME) - 1) == 0) {   \
                                                                            \
            return TypeSpec::RTYPEWORD##Mask;                               \
        }
    #include "pseudofuncs.dsl.h"
    #undef DEF_PFUNC

    ERR("Unknown pseudo-function, return type indeterminable: \"%.*s\"", name->getLength(), name->getStr());

    return TypeSpec::NoneMask;
}

#define DEF_TYPE(NAME) \
    bool Expression::is##NAME() const { \
        return VISINST(this, NAME); \
    }
#include "exprtypes.dsl.h"
#undef DEF_TYPE

void Expression::VMIN(Void, dtor)() {
}

void Expression::VMIN(Asgn, dtor)() {
    am = Expression::AM_EQ;
}

void Expression::VMIN(PolyOp, dtor)() {
    ops.dtor();
}

void Expression::VMIN(Neg, dtor)() {
}

void Expression::VMIN(Cast, dtor)() {
    cast.dtor();
}

void Expression::VMIN(Num, dtor)() {
    num = nullptr;
}

void Expression::VMIN(VarRef, dtor)() {
    name = nullptr;
}

void Expression::VMIN(FuncCall, dtor)() {
    name = nullptr;
}


TypeSpec::Mask Expression::VMIN(Void, deduceType)(Scope *, const Program *) {
    return TypeSpec::VoidMask;
}

TypeSpec::Mask Expression::VMIN(Asgn, deduceType)(Scope *scope, const Program *prog) {
    assert(children.getSize() == 2);

    typeMask &= children[0].deduceType(typeMask, scope, prog);  // Result is always either unambiguous or none
    typeMask &= children[1].deduceType(typeMask, scope, prog);

    // So, whenever typeMask ends up unambiguous, the children already know about it

    return typeMask;
}

TypeSpec::Mask Expression::VMIN(PolyOp, deduceType)(Scope *scope, const Program *prog) {
    bool isCmp = false;

    if (ops.getSize() > 0) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wswitch-enum"

        switch (ops[0]) {
        case Expression::OP_EQ:
        case Expression::OP_NEQ:
        case Expression::OP_GEQ:
        case Expression::OP_LEQ:
        case Expression::OP_GT:
        case Expression::OP_LT:
            isCmp = true;  // A comparison always return Int4, no matter what is consists of, but its children should still be of a single and determinable type
            typeMask = TypeSpec::AllMask & ~TypeSpec::VoidMask;

            if (children.getSize() > 2) {
                ERR("Comparisons of more than two values aren't yet implemented, sorry");

                return TypeSpec::NoneMask;
            }

            break;

        default:
            break;
        }

        #pragma GCC diagnostic pop
    }

    for (unsigned i = 0; typeMask && i < children.getSize(); ++i) {
        typeMask &= children[i].deduceType(typeMask, scope, prog);
    }

    if (TypeSpec::isMaskUnambiguous(typeMask)) {
        for (unsigned i = 0; i < children.getSize(); ++i) {
            children[i].deduceType(typeMask, scope, prog);  // Pushing through for them to know who to compile as
        }
    }

    if (isCmp) {
        if (TypeSpec::isMaskUnambiguous(typeMask)) {
            typeMask = TypeSpec::Int4Mask;
        } else {
            typeMask = TypeSpec::NoneMask;
        }
    }

    return typeMask;
}

TypeSpec::Mask Expression::VMIN(Neg, deduceType)(Scope *scope, const Program *prog) {
    assert(children.getSize() == 1);

    return children[0].deduceType(typeMask, scope, prog);  // Again, no manual pushing required
}

TypeSpec::Mask Expression::VMIN(Cast, deduceType)(Scope *scope, const Program *prog) {
    assert(children.getSize() == 1);

    /*
    // Actually, this should probably be done during compilation, not type deduction
    if (!children[0].deduceType(TypeSpec::AllMask & ~TypeSpec::VoidMask, scope, prog)) {
        return TypeSpec::NoneMask;
    }
    */

    // The only requirement posed on the cast subject is to be non-void (at least at this point)
    return cast.getMask() & children[0].deduceType(TypeSpec::AllMask & ~TypeSpec::VoidMask, scope, prog);
}

TypeSpec::Mask Expression::VMIN(Num, deduceType)(Scope *, const Program *) {
    assert(num->isNum());

    if (num->isInteger()) {
        return TypeSpec::Int4Mask | TypeSpec::Int8Mask /*| TypeSpec::DblMask*/;  // TODO: Not sure if we should consider double possible here
    } else {
        return TypeSpec::DblMask;
    }
}

TypeSpec::Mask Expression::VMIN(VarRef, deduceType)(Scope *scope, const Program *) {
    assert(name->isName());

    VarInfo vi = scope->getInfo(name);

    if (!vi.var) {
        ERR("Unknown variable, type indeterminable: \"%.*s\"", name->getLength(), name->getStr());

        return TypeSpec::NoneMask;
    }

    return vi.var->getType().getMask();
}

TypeSpec::Mask Expression::VMIN(FuncCall, deduceType)(Scope *, const Program *prog) {
    assert(name->isName());

    if (name->getLength() >= 1 && name->getStr()[0] == '_') {
        return getPseudofuncRtypeMask();
    }

    const Function *func = prog->getFunction(name);

    if (!func) {
        ERR("Unknown function, return type indeterminable: \"%.*s\"", name->getLength(), name->getStr());

        return TypeSpec::NoneMask;
    }

    // Argument masks and count checks should be applied later

    return func->getRtype().getMask();  // TODO: ?
}


void Expression::VMIN(Void, reconstruct)(FILE *ofile) const {
    fprintf(ofile, "???");
    ERR("Void expressions don't exist");
    assert(false);
}

void Expression::VMIN(Asgn, reconstruct)(FILE *ofile) const {
    children[0].reconstruct(ofile);

    switch (am) {
    case Expression::AM_EQ:
        fprintf(ofile, " = ");
        break;

    case Expression::AM_ADDEQ:
        fprintf(ofile, " += ");
        break;

    case Expression::AM_SUBEQ:
        fprintf(ofile, " -= ");
        break;

    case Expression::AM_MULEQ:
        fprintf(ofile, " -= ");
        break;

    case Expression::AM_DIVEQ:
        fprintf(ofile, " /= ");
        break;

    case Expression::AM_MODEQ:
        fprintf(ofile, " %%= ");
        break;

    NODEFAULT
    }

    children[1].reconstruct(ofile);
}

void Expression::VMIN(PolyOp, reconstruct)(FILE *ofile) const {
    assert(children.getSize() > 0);

    children[0].reconstruct(ofile);

    for (unsigned i = 1; i < children.getSize(); ++i) {
        switch (ops[i - 1]) {
        case OP_EQ:
            fprintf(ofile, " == ");
            break;

        case OP_NEQ:
            fprintf(ofile, " != ");
            break;

        case OP_GEQ:
            fprintf(ofile, " >= ");
            break;

        case OP_LEQ:
            fprintf(ofile, " <= ");
            break;

        case OP_LT:
            fprintf(ofile, " < ");
            break;

        case OP_GT:
            fprintf(ofile, " > ");
            break;

        case OP_ADD:
            fprintf(ofile, " + ");
            break;

        case OP_SUB:
            fprintf(ofile, " - ");
            break;

        case OP_MUL:
            fprintf(ofile, " * ");
            break;

        case OP_DIV:
            fprintf(ofile, "/ ");
            break;

        case OP_MOD:
            fprintf(ofile, " %% ");
            break;

        NODEFAULT
        }

        children[i].reconstruct(ofile);
    }
}

void Expression::VMIN(Neg, reconstruct)(FILE *ofile) const {
    fprintf(ofile, "-");

    children[0].reconstruct(ofile);
}

void Expression::VMIN(Cast, reconstruct)(FILE *ofile) const {
    cast.reconstruct(ofile);

    children[0].reconstruct(ofile);
}

void Expression::VMIN(Num, reconstruct)(FILE *ofile) const {
    if (num->isInteger()) {
        fprintf(ofile, "%llu", num->asInt());
    } else {
        double tmp = num->asDbl();

        fprintf(ofile, "%lg", tmp);

        if (tmp == (double)(unsigned long long)tmp) {
            fprintf(ofile, ".");
        }
    }
}

void Expression::VMIN(VarRef, reconstruct)(FILE *ofile) const {
    fprintf(ofile, "%.*s", name->getLength(), name->getStr());
}

void Expression::VMIN(FuncCall, reconstruct)(FILE *ofile) const {
    fprintf(ofile, "%.*s(", name->getLength(), name->getStr());

    for (unsigned i = 0; i < children.getSize(); ++i) {
        children[i].reconstruct(ofile);
    }

    fprintf(ofile, ")");
}


#define DEF_TYPE(NAME) \
    VTYPE_DEF(NAME, Expression) = { \
        Expression::VMIN(NAME, dtor), \
        Expression::VMIN(NAME, deduceType), \
        Expression::VMIN(NAME, compile), \
        Expression::VMIN(NAME, reconstruct), \
    };
#include "exprtypes.dsl.h"
#undef DEF_TYPE

//================================================================================

bool Code::ctor() {
    TRY_B(stmts.ctor());
    TRY_B(scope.ctor());

    return false;
}

void Code::dtor() {
    stmts.dtor();
    scope.dtor();
}

bool Code::makeStatement(Statement** stmt) {
    TRY_B(stmts.append());

    *stmt = &stmts[-1];

    return false;
}

void Code::popStatement() {
    stmts.pop();
}

void Code::simplifyLastEmpty() {
    assert(stmts.getSize() > 0);

    if (stmts[-1].isEmpty()) {
        stmts[-1].dtor();
        stmts.pop();
    }
}

Scope *Code::getScope() {
    return &scope;
}

void Code::reconstruct(FILE *ofile, unsigned indent) const {
    for (unsigned i = 0; i < stmts.getSize(); ++i) {
        stmts[i].reconstruct(ofile, indent);
    }
}

//================================================================================

bool Statement::ctor() {
    VSETTYPE(this, Empty);

    return false;
}

bool Statement::ctorCompound() {
    VSETTYPE(this, Compound);
    TRY_B(code.ctor());

    return false;
}

bool Statement::ctorReturn() {
    VSETTYPE(this, Return);
    TRY_B(expr.ctor());

    return false;
}

bool Statement::ctorLoop() {
    VSETTYPE(this, Loop);
    TRY_B(expr.ctor());
    TRY_B(code.ctor());

    return false;
}

bool Statement::ctorCond() {
    VSETTYPE(this, Cond);
    TRY_B(expr.ctor());
    TRY_B(code.ctor());
    TRY_B(altCode.ctor());

    return false;
}

bool Statement::ctorVarDecl() {
    VSETTYPE(this, VarDecl);
    TRY_B(var.ctor());
    TRY_B(expr.ctor());

    return false;
}

bool Statement::ctorExpr() {
    VSETTYPE(this, Expr);
    TRY_B(expr.ctor());

    return false;
}

bool Statement::ctorEmpty() {
    VSETTYPE(this, Empty);

    return false;
}

void Statement::dtor() {
    if (vtable_)
        VCALL(this, dtor);
}

bool Statement::makeCode(Code** out_code) {
    *out_code = &code;

    return false;
}

bool Statement::makeAltCode(Code** out_altCode) {
    *out_altCode = &altCode;

    return false;
}

bool Statement::makeExpr(Expression** out_expr) {
    *out_expr = &expr;

    return false;
}

bool Statement::makeVar(Var** out_var) {
    *out_var = &var;

    return false;
}

void Statement::reconstruct(FILE *ofile, unsigned indent) const {
    for (unsigned i = 0; i < indent; ++i)
        fprintf(ofile, "    ");

    VCALL(this, reconstruct, ofile, indent);

    fprintf(ofile, "\n");
}

#define DEF_TYPE(NAME) \
    bool Statement::is##NAME() const { \
        return VISINST(this, NAME); \
    }
#include "stmttypes.dsl.h"
#undef DEF_TYPE

void Statement::VMIN(Compound, dtor)() {
    code.dtor();
}

void Statement::VMIN(Return, dtor)() {
    expr.dtor();
}

void Statement::VMIN(Loop, dtor)() {
    expr.dtor();
    code.dtor();
}

void Statement::VMIN(Cond, dtor)() {
    expr.dtor();
    code.dtor();
    altCode.dtor();
}

void Statement::VMIN(VarDecl, dtor)() {
    var.dtor();
    expr.dtor();
}

void Statement::VMIN(Expr, dtor)() {
    expr.dtor();
}

void Statement::VMIN(Empty, dtor)() {
}

void Statement::VMIN(Compound, reconstruct)(FILE *ofile, unsigned indent) const {
    fprintf(ofile, "{\n");
    code.reconstruct(ofile, indent + 1);

    for (unsigned i = 0; i < indent; ++i)
        fprintf(ofile, "    ");

    fprintf(ofile, "}");
}

void Statement::VMIN(Return, reconstruct)(FILE *ofile, unsigned) const {
    fprintf(ofile, "ret");

    if (!expr.isVoid()) {
        fprintf(ofile, " ");
        expr.reconstruct(ofile);
    }

    fprintf(ofile, ";");
}

void Statement::VMIN(Loop, reconstruct)(FILE *ofile, unsigned indent) const {
    fprintf(ofile, "while ");

    expr.reconstruct(ofile);

    fprintf(ofile, " {\n");

    code.reconstruct(ofile, indent + 1);

    for (unsigned i = 0; i < indent; ++i)
        fprintf(ofile, "    ");

    fprintf(ofile, "}");
}

void Statement::VMIN(Cond, reconstruct)(FILE *ofile, unsigned indent) const {
    fprintf(ofile, "if ");

    expr.reconstruct(ofile);

    fprintf(ofile, " {\n");

    code.reconstruct(ofile, indent + 1);

    for (unsigned i = 0; i < indent; ++i)
        fprintf(ofile, "    ");

    fprintf(ofile, "} else {\n");

    altCode.reconstruct(ofile, indent + 1);

    for (unsigned i = 0; i < indent; ++i)
        fprintf(ofile, "    ");

    fprintf(ofile, "}");
}

void Statement::VMIN(VarDecl, reconstruct)(FILE *ofile, unsigned) const {
    fprintf(ofile, "var ");
    var.getType().reconstruct(ofile);
    fprintf(ofile, "%.*s", var.getName()->getLength(), var.getName()->getStr());

    if (!expr.isVoid()) {
        fprintf(ofile, " = ");

        expr.reconstruct(ofile);
    }

    fprintf(ofile, ";");
}

void Statement::VMIN(Expr, reconstruct)(FILE *ofile, unsigned) const {
    expr.reconstruct(ofile);
    fprintf(ofile, ";");
}

void Statement::VMIN(Empty, reconstruct)(FILE *ofile, unsigned) const {
    fprintf(ofile, ";");
}

#define DEF_TYPE(NAME) \
    VTYPE_DEF(NAME, Statement) = { \
        Statement::VMIN(NAME, dtor), \
        Statement::VMIN(NAME, compile), \
        Statement::VMIN(NAME, reconstruct), \
    };
#include "stmttypes.dsl.h"
#undef DEF_TYPE

//================================================================================

bool Function::ctor() {
    TRY_B(args.ctor());
    TRY_B(code.ctor());
    type = T_DEF;
    hasCode = false;
    rtype = {};
    name = nullptr;

    return false;
}

bool Function::ctor(TypeSpec new_rtype, const Token* new_name) {
    TRY_B(ctor());

    rtype = new_rtype;
    name = new_name;

    return false;
}

void Function::dtor() {
    args.dtor();
    code.dtor();
    rtype = {};
    name = nullptr;
}

bool Function::makeArg(Var** arg) {
    TRY_B(args.append({}));

    *arg = &args[-1];

    return false;
}

void Function::popArg() {
    args.pop();
}

bool Function::makeCode(Code** out_code) {
    *out_code = &code;

    return false;
}

bool Function::registerArgs() {
    for (unsigned i = 0; i < args.getSize(); ++i) {
        TRY_B(code.getScope()->addArg(&args[i], i));
    }

    return false;
}

bool Function::isMain() const {
    const char MAIN_NAME[] = "main";

    return type == T_DEF && hasCode &&
           name->getLength() == sizeof(MAIN_NAME) &&
           rtype.type == TypeSpec::T_VOID &&
           strncmp(name->getStr(), MAIN_NAME, sizeof(MAIN_NAME)) == 0;
}

TypeSpec Function::getRtype() const {
    return rtype;
}

const Token *Function::getName() const {
    return name;
}

const Vector<Var> *Function::getArgs() const {
    return &args;
}

void Function::reconstruct(FILE *ofile) const {
    fprintf(ofile, "def %.*s(", name->getLength(), name->getStr());

    const Token *curName = nullptr;

    if (args.getSize() >= 1) {
        curName = args[0].getName();

        fprintf(ofile, "%.*s", curName->getLength(), curName->getStr());
    }

    for (unsigned i = 1; i < args.getSize(); ++i) {
        curName = args[i].getName();

        fprintf(ofile, ", %.*s", curName->getLength(), curName->getStr());
    }

    fprintf(ofile, ") {\n");

    code.reconstruct(ofile, 1);

    fprintf(ofile, "}\n\n");
}

void Function::setType(type_e new_type) {
    type = new_type;

    if (type == T_C_EXPORT) {
        code.getScope()->allocSavedSpace(64);  // R12, R13, R14, R15, RDI, RSI, RBX + one spare cell for alignment
    }
}

//================================================================================

bool Program::ctor() {
    TRY_B(functions.ctor());

    return false;
}

void Program::dtor() {
    functions.dtor();
}

bool Program::makeFunction(Function** dest) {
    TRY_B(functions.append());

    *dest = &functions[-1];

    return false;
}

void Program::popFunction() {
    functions.pop();
}

const Function *Program::getFunction(const Token *name) const {
    for (unsigned i = 0; i < functions.getSize(); ++i) {
        const Token *curName = functions[i].getName();

        // TODO: Encapsulate name token comparison into the token itself
        if (name->getLength() == curName->getLength() &&
            strncmp(name->getStr(), curName->getStr(), name->getLength()) == 0) {

            return &functions[i];
        }
    }

    return nullptr;
}

void Program::reconstruct(FILE *ofile) const {
    for (unsigned i = 0; i < functions.getSize(); ++i) {
        functions[i].reconstruct(ofile);
    }
}

}

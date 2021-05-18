#include "object.h"

#include "hashtable.h"

#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include <algorithm>  // For std::sort


namespace abel {

bool PackedInstruction::setPrefixes(const uint8_t new_prefixes[4]) {  // The unused ones have to be null
    unsigned cur = flags.getPrefCnt();

    for (unsigned i = 0; i < 4; ++i) {
        if (!new_prefixes[i])
            continue;

        if (cur >= 4) {
            ERR("Too many instruction prefixes");

            return true;
        }

        prefixes[cur++] = new_prefixes[i];
    }

    flags.setPrefCnt(cur);

    return false;
}

bool PackedInstruction::setRawOp(const uint8_t new_op[3], unsigned opcodeSize) {
    if (flags.getType() != T_PLAIN) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (opcodeSize > 3) {
        ERR("Opcode too big");
        return true;
    }

    flags.setOpcodeSize(opcodeSize);
    memcpy(rawOp, new_op, opcodeSize);

    return false;
}

bool PackedInstruction::setRexOp(const uint8_t new_op[3], unsigned opcodeSize) {
    if (flags.getType() != T_REX) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (opcodeSize > 3) {
        ERR("Opcode too big");
        return true;
    }

    flags.setOpcodeSize(opcodeSize);
    memcpy(rex.op, new_op, opcodeSize);

    return false;
}

bool PackedInstruction::setRawOpVariant(reg_e reg) {
    if (flags.getType() != T_PLAIN) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (flags.getOpcodeSize() != 1) {
        ERR("Opcode too big");
        return true;
    }

    if (reg >= REG_8) {
        ERR("Register needs REX prefix");
        return true;
    }

    rawOp[0] = (rawOp[0] & ~0b111) | (reg & 0b111);

    return false;
}

bool PackedInstruction::setRexOpVariant(reg_e reg) {
    if (flags.getType() != T_REX) {
        ERR("Use a method corresponding to your opcode type instead");
        return true;
    }

    if (flags.getOpcodeSize() != 1) {
        ERR("Opcode too big");
        return true;
    }

    rex.op[0] = (rex.op[0] & ~0b111) | (reg & 0b111);
    rex.B = reg >= REG_8;

    return false;
}

bool PackedInstruction::ctor() {
    // TODO: ?

    return false;
}

void PackedInstruction::dtor() {
}

void PackedInstruction::hexDump() const {
    for (unsigned i = 0; i < flags.getPrefCnt(); ++i) {
        printf("%02x ", prefixes[i]);
    }

    switch (flags.getType()) {
    case T_PLAIN:
        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02x", rawOp[i]);
        }

        break;

    case T_REX:
        printf("%02x ", rex.rex);

        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            printf("%02x", rex.op[i]);
        }

        break;

    case T_VEX2:
        break;

    case T_VEX3:
        break;

    NODEFAULT
    }
    printf(" ");

    if (flags.hasModrm()) {
        printf("%02x ", modrm.full);
    }

    if (flags.hasSib()) {
        assert(flags.hasModrm());
        printf("%02x ", sib.full);
    }

    #define ITH_BYTE_(NUM, I)  (unsigned)((NUM >> (I * 8)) & 0xFF)

    switch (flags.getDispSize()) {
    case -1u:
        break;

    case 0:
        printf("%02x ", ITH_BYTE_(displacement, 0));
        break;

    case 1:
        printf("%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1));
        break;

    case 2:
        printf("%02x%02x%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1),
                                    ITH_BYTE_(displacement, 2), ITH_BYTE_(displacement, 3));
        break;

    case 3:
        printf("%02x%02x%02x%02x%02x%02x%02x%02x ", ITH_BYTE_(displacement, 0), ITH_BYTE_(displacement, 1),
                                                    ITH_BYTE_(displacement, 2), ITH_BYTE_(displacement, 3),
                                                    ITH_BYTE_(displacement, 4), ITH_BYTE_(displacement, 5),
                                                    ITH_BYTE_(displacement, 6), ITH_BYTE_(displacement, 7));
        break;

    NODEFAULT
    }

    switch (flags.getImmSize()) {
    case -1u:
        break;

    case 0:
        printf("%02x ", ITH_BYTE_(immediate, 0));
        break;

    case 1:
        printf("%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1));
        break;

    case 2:
        printf("%02x%02x%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1),
                                    ITH_BYTE_(immediate, 2), ITH_BYTE_(immediate, 3));
        break;

    case 3:
        printf("%02x%02x%02x%02x%02x%02x%02x%02x ", ITH_BYTE_(immediate, 0), ITH_BYTE_(immediate, 1),
                                                    ITH_BYTE_(immediate, 2), ITH_BYTE_(immediate, 3),
                                                    ITH_BYTE_(immediate, 4), ITH_BYTE_(immediate, 5),
                                                    ITH_BYTE_(immediate, 6), ITH_BYTE_(immediate, 7));
        break;

    NODEFAULT
    }

    #undef ITH_BYTE_

    printf("\n");
}

unsigned PackedInstruction::getLength() const {
    return flags.getPrefCnt()
         + flags.getOpcodeSize()
         + flags.hasModrm()
         + flags.hasSib()
         + (flags.getDispSize() != -1u ? 1 << flags.getDispSize() : 0)
         + (flags.getImmSize()  != -1u ? 1 << flags.getImmSize()  : 0);
}

bool PackedInstruction::compile(char **dest, unsigned limit) const {
    TRY_B(getLength() > limit);

    char *oldDest = *dest;

    #define NEXT_BYTE_  *(*dest)++

    for (unsigned i = 0; i < flags.getPrefCnt(); ++i) {
        NEXT_BYTE_ = prefixes[i];
    }

    switch (flags.getType()) {
    case T_PLAIN:
        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            NEXT_BYTE_ = rawOp[i];
        }

        break;

    case T_REX:
        NEXT_BYTE_ = rex.rex;

        for (unsigned i = 0; i < flags.getOpcodeSize(); ++i) {
            NEXT_BYTE_ = rex.op[i];
        }

        break;

    case T_VEX2:
    case T_VEX3:
        ERR("Can't yet compile VEX instructions");

        goto err;

    NODEFAULT
    }

    if (flags.hasModrm()) {
        NEXT_BYTE_ = modrm.full;
    }

    if (flags.hasSib()) {
        assert(flags.hasModrm());

        NEXT_BYTE_ = sib.full;
    }

    #define ITH_BYTE_(NUM, I)  (unsigned)((NUM >> (I * 8)) & 0xFF)

    if (flags.getDispSize() != -1u) {
        unsigned dispSize = 1 << flags.getDispSize();

        for (unsigned i = 0; i < dispSize; ++i) {
            NEXT_BYTE_ = ITH_BYTE_(displacement, i);
        }
    }

    if (flags.getImmSize() != -1u) {
        unsigned immSize = 1 << flags.getImmSize();

        for (unsigned i = 0; i < immSize; ++i) {
            NEXT_BYTE_ = ITH_BYTE_(immediate, i);
        }
    }

    #undef ITH_BYTE_

    #undef NEXT_BYTE_

    return false;

err:
    *dest = oldDest;
    return true;
}

//================================================================================

bool FuncInfo::ctor() {
    TRY_B(symbol.ctor());

    isImport = false;
    isStatic = false;

    addr = -1u;

    return false;
}

void FuncInfo::dtor() {
    symbol.dtor();
}

bool FuncInfo::setFunction(const char *name, unsigned length) {
    return symbol.ctorFunction(name, length);
}

bool FuncInfo::setExpFunction(const char *name, unsigned length, unsigned new_addr) {
    setExport(new_addr);
    return setFunction(name, length);
}

bool FuncInfo::setImpFunction(const char *name, unsigned length) {
    setImport();
    return setFunction(name, length);
}

//================================================================================

ObjectFactory::result_e ObjectFactory::ctor() {
    lastResult = R_OK;

    if (code.ctor() || funcs.ctor() || labelAliases.ctor()) {
        ERR("Couldn't init underlying Vector");
        return lastResult = R_BADMEMORY;
    }

    nextLabelIdx = 0;

    if (scheduledLabel.ctor()) {
        ERR("Couldn't init underlying Symbol");
        return lastResult = R_BADMEMORY;
    }

    stkCurTos       = 0;
    stkCurSize      = 0;
    stkCurMem       = 0;
    stkWasAligned   = false;

    bypass = false;

    return lastResult = R_OK;
}

void ObjectFactory::dtor() {
    code.dtor();
    funcs.dtor();
    scheduledLabel.dtor();
}

void ObjectFactory::stkReset() {
    stkCurTos       = 0;
    stkCurSize      = 0;
    stkCurMem       = 0;
    stkWasAligned   = 0;
}

reg_e ObjectFactory::stkTos(unsigned depth) const {
    REQUIRE(depth <= stkCurSize || bypass);

    return REGSTK_REGS[(REGSTK_SIZE + stkCurTos - depth) % REGSTK_SIZE];
}

ObjectFactory::result_e ObjectFactory::stkPush() {
    if (stkCurSize == REGSTK_SIZE) {
        bypass = true;

        stkPushMem();

        getLastInstr().setOp(Opcode_e::push_rm64)
                      .setRmReg(stkTos(0));

        stkCurSize--;

        bypass = false;
    }

    stkCurTos = (stkCurTos + 1) % REGSTK_SIZE;
    stkCurSize++;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkPop() {
    if (stkCurSize == 0) {
        assert(false);  // People should pull before popping

        return lastResult = R_BADCONTRACT;
    }

    stkCurTos = (REGSTK_SIZE + stkCurTos - 1) % REGSTK_SIZE;
    stkCurSize--;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkFlush() {
    bypass = true;

    stkPushMem(stkCurSize);

    for (unsigned i = 0; i < stkCurSize; ++i) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::push_rm64)
                      .setRmReg(stkTos(stkCurSize - i));
    }

    stkCurSize = 0;
    stkCurTos = 0;

    bypass = false;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkFlushExceptOne() {
    assert(stkCurSize != 0);  // Should probably be the case

    if (stkCurSize == 0) {
        TRY(stkPull(1));
    }

    bypass = true;

    stkPushMem(stkCurSize - 1);

    for (unsigned i = 0; i < stkCurSize - 1; ++i) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::push_rm64)
                      .setRmReg(stkTos(stkCurSize - i));
    }

    stkCurSize = 1;
    stkCurTos = (stkCurTos - (stkCurSize - 1) + REGSTK_SIZE) % REGSTK_SIZE;

    if (stkCurTos != 1) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::mov_rm64_r64)
                      .setRmReg(REGSTK_REGS[0])
                      .setR(stkTos());

        stkCurTos = 1;
    }

    bypass = false;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkPull(unsigned req) {
    assert(req < REGSTK_SIZE);

    if (stkCurSize >= req)
        return lastResult = R_OK;

    bypass = true;

    stkPopMem(req - stkCurSize);
    stkCurTos = (stkCurTos + req - stkCurSize) % REGSTK_SIZE;

    for (unsigned i = 0; i < req - stkCurSize; ++i) {
        TRY(addInstr());
        getLastInstr().setOp(Opcode_e::pop_rm64)
                      .setRmReg(stkTos(i + 1));
    }

    stkCurSize = req;

    bypass = false;

    return lastResult = R_OK;
}

void ObjectFactory::stkPushMem(unsigned count) {
    //assert(!stkWasAligned);
    stkCurMem += count;
}

void ObjectFactory::stkPopMem(unsigned count) {
    //assert(!stkWasAligned);
    stkCurMem -= count;
}

ObjectFactory::result_e ObjectFactory::stkAlign(unsigned frameSize, unsigned nArgs) {
    if (stkIsAligned() ^ (bool)((frameSize + nArgs * 8) & 15))
        return lastResult = R_OK;

    assert(!stkWasAligned);

    TRY(addInstr());
    getLastInstr().setOp(Opcode_e::sub_rm64_imm8)
                  .setRmReg(REG_SP)
                  .setImm(8);

    stkWasAligned = true;

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::stkUnalign() {
    if (!stkWasAligned)
        return lastResult = R_OK;

    TRY(addInstr());
    getLastInstr().setOp(Opcode_e::add_rm64_imm8)
                  .setRmReg(REG_SP)
                  .setImm(8);

    stkWasAligned = false;

    return lastResult = R_OK;
}

unsigned ObjectFactory::reserveLabel() {
    return nextLabelIdx++;
}

unsigned ObjectFactory::placeLabel() {
    unsigned label = reserveLabel();

    return placeLabel(label) ? -1u : label;
}

ObjectFactory::result_e ObjectFactory::placeLabel(unsigned reservedLabelIdx) {
    if (scheduledLabel.isUsed()) {
        if (labelAliases.append({reservedLabelIdx, scheduledLabel.getLabelIdx()}))
            return lastResult = R_BADMEMORY;

        return lastResult = R_OK;
    }

    if (scheduledLabel.ctorLabel(reservedLabelIdx)) {
        return lastResult = R_BADSYMBOL;
    }

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::defineFunction(const Token *name, bool mangle, bool isExport) {
    assert(name);
    assert(name->isName());

    return defineFunction(name->getStr(), name->getLength(), mangle, isExport);
}

ObjectFactory::result_e ObjectFactory::defineFunction(const char *name, unsigned length, bool mangle, bool isExport) {
    assert(name);

    if (funcs.append())
        return lastResult = R_BADMEMORY;

    if (mangle) {
        char *newName = (char *)calloc(length + Symbol::PREFIX_LEN + 1, sizeof(char));
        if (!newName)
            return lastResult = R_BADMEMORY;

        sprintf(newName, "%s%.*s", Symbol::PREFIX_FUNCTION, length, name);

        bool result = funcs[-1].setExpFunction(newName, length + Symbol::PREFIX_LEN, code.getSize());

        free(newName);

        if (result)
            return lastResult = R_BADMEMORY;

    } else {
        if (funcs[-1].setExpFunction(name, length, code.getSize()))
            return lastResult = R_BADMEMORY;
    }

    funcs[-1].setStatic(!isExport);

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::importFunction(const Token *name, bool mangle) {
    assert(name);
    assert(name->isName());

    return importFunction(name->getStr(), name->getLength(), mangle);
}

ObjectFactory::result_e ObjectFactory::importFunction(const char *name, unsigned length, bool mangle) {
    assert(name);

    if (funcs.append())
        return lastResult = R_BADMEMORY;

    if (mangle) {
        char *newName = (char *)calloc(length + Symbol::PREFIX_LEN + 1, sizeof(char));
        if (!newName)
            return lastResult = R_BADMEMORY;

        sprintf(newName, "%s%.*s", Symbol::PREFIX_FUNCTION, length, name);

        bool result = funcs[-1].setImpFunction(newName, length + Symbol::PREFIX_LEN);

        free(newName);

        if (result)
            return lastResult = R_BADMEMORY;

    } else {
        if (funcs[-1].setImpFunction(name, length))
            return lastResult = R_BADMEMORY;
    }

    return lastResult = R_OK;
}

ObjectFactory::result_e ObjectFactory::addInstr() {
    if (code.append() || code[-1].ctor()) {
        return lastResult = R_BADMEMORY;
    }

    if (scheduledLabel.isUsed()) {
        Symbol::swap(*(code[-1].getSymbolHere()), scheduledLabel);
        scheduledLabel.dtor();
        if (scheduledLabel.ctorNone()) {
            return lastResult = R_BADMEMORY;
        }
    }

    return lastResult = R_OK;
}

Instruction &ObjectFactory::getLastInstr() {
    REQUIRE(code.getSize() > 0);

    return code[-1];
}

void ObjectFactory::dump() const {
    for (unsigned i = 0; i < code.getSize(); ++i) {
        if (code[i].isRemoved())
            continue;

        PackedInstruction pi{};

        REQUIRE(!code[i].compile(pi));

        pi.hexDump();

        pi.dtor();
    }
}

static bool generateReloc(const Symbol *symbol, Vector<IMAGE_RELOCATION> &relocs,
                          unsigned virtAddr, Hashtable<unsigned> &symbolLookup,
                          unsigned ripOffset) {

    TRY_B(!symbol);

    if (!symbol->isUsed())
        return false;

    TRY_B(ripOffset - 4 > 5);

    TRY_B(relocs.append());
    IMAGE_RELOCATION &curReloc = relocs[-1];

    unsigned symbolIndex = 0;

    switch (symbol->getType()) {
    case Symbol::T_LABEL: {
        char buf[IMAGE_SIZEOF_SHORT_NAME + 1] = "";

        TRY_B(symbol->composeLabelName(buf, IMAGE_SIZEOF_SHORT_NAME + 1));

        TRY_BC(symbolLookup.get(buf, &symbolIndex),
               ERR("Unknown label #%u referenced", symbol->getLabelIdx()));

    } break;

    case Symbol::T_FUNCTION: {
        TRY_BC(symbolLookup.get(symbol->getFunctionName(), symbol->getFunctionNameLength(), &symbolIndex),
               ERR("Unknown function '%.*s' referenced", symbol->getFunctionNameLength(), symbol->getFunctionName()));

    } break;

    case Symbol::T_NONE:
    NODEFAULT
    }

    curReloc.VirtualAddress = virtAddr;
    curReloc.SymbolTableIndex = symbolIndex;
    curReloc.Type = IMAGE_REL_AMD64_REL32 + ripOffset - 4;

    return false;
}

ObjectFactory::result_e ObjectFactory::compile(FILE *ofile) const {
    #define ERR_(CODE)  { lastResult = CODE; goto err; }

    #define WRITE_(SRC, SIZE, CNT)                  \
        if (fwrite(SRC, SIZE, CNT, ofile) != CNT)   \
            ERR_(R_BADIO);

    constexpr unsigned SECTIONS_COUNT = 1;  // .text
    constexpr unsigned SECT_TEXT = 1;


    IMAGE_FILE_HEADER coffHeader{};
    Vector<IMAGE_SECTION_HEADER> sectionTable{};
    Vector<IMAGE_SYMBOL> symbolTable{};
    Vector<char> strings{};  // Not exactly string table yet
    Vector<IMAGE_RELOCATION> textRelocs{};
    Vector<char> textRawData{};
    Vector<unsigned> auxSymIdx{};

    struct FuncAddr_ {
        unsigned symIdx     = -1u;
        unsigned instrIdx   = -1u;

        bool operator<(const FuncAddr_ &other) const {
            return instrIdx < other.instrIdx;
        }
    };
    Vector<FuncAddr_> funcAddrs{};

    Hashtable<unsigned> symbolLookup{};

    IMAGE_SECTION_HEADER *sectText = nullptr;

    unsigned curPos = 0;
    unsigned instrIdx = 0;
    unsigned funcAddrIdx = 0;

    if (!ofile) ERR_(R_BADPTR);

    if (sectionTable.ctor(SECTIONS_COUNT))      ERR_(R_BADMEMORY);
    if (symbolTable.ctor())                     ERR_(R_BADMEMORY);
    if (strings.ctor())                         ERR_(R_BADMEMORY);
    if (textRelocs.ctor())                      ERR_(R_BADMEMORY);
    if (textRawData.ctor())                     ERR_(R_BADMEMORY);
    if (auxSymIdx.ctor(SECTIONS_COUNT))         ERR_(R_BADMEMORY);
    if (funcAddrs.ctor())                       ERR_(R_BADMEMORY);

    if (symbolLookup.ctor())                    ERR_(R_BADMEMORY);

    sectText = &sectionTable[SECT_TEXT - 1];

    coffHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
    coffHeader.NumberOfSections = SECTIONS_COUNT;
    coffHeader.TimeDateStamp = (DWORD)time(nullptr);
    // coffHeader.PointerToSymbolTable = ;
    // coffHeader.NumberOfSymbols = ;
    coffHeader.SizeOfOptionalHeader = 0;
    coffHeader.Characteristics = 0;

    // Warning: if the name is 8 characters long, use memcpy instead.
    // If it's longer than 8 characters, the string table should be used
    strcpy((char *)sectText->Name, ".text");
    sectText->Misc.VirtualSize = 0;
    sectText->VirtualAddress = 0;
    // sectText->SizeOfRawData = ;
    // sectText->PointerToRawData = ;
    // sectText->PointerToRelocations = ;
    sectText->PointerToLinenumbers = 0;
    // sectText->NumberOfRelocations = ;
    sectText->NumberOfLinenumbers = 0;
    sectText->Characteristics =
        IMAGE_SCN_CNT_CODE |
        // ?IMAGE_SCN_LNK_NRELOC_OVFL |
        IMAGE_SCN_MEM_EXECUTE |
        IMAGE_SCN_MEM_READ;

    for (unsigned i = 0; i < sectionTable.getSize(); ++i) {  // Symbols for sections themselves
        const IMAGE_SECTION_HEADER &cur = sectionTable[i];

        if (symbolTable.append() || symbolTable.append()) ERR_(R_BADMEMORY);

        IMAGE_SYMBOL &symbolEntry = symbolTable[-2];
        IMAGE_AUX_SYMBOL &auxEntry = reinterpret_cast<IMAGE_AUX_SYMBOL &>(symbolTable[-1]);

        memcpy(symbolEntry.N.ShortName, cur.Name, IMAGE_SIZEOF_SHORT_NAME);
        symbolEntry.Value = 0;
        symbolEntry.SectionNumber = i + 1;
        symbolEntry.Type = 0;
        symbolEntry.StorageClass = IMAGE_SYM_CLASS_STATIC;
        symbolEntry.NumberOfAuxSymbols = 1;

        auxSymIdx[i] = symbolTable.getSize() - 1;

        // auxEntry.Section.Length = ;
        // auxEntry.Section.NumberOfRelocations = ;
        // auxEntry.Section.NumberOfLinenumbers = ;
        auxEntry.Section.CheckSum = 0;
        auxEntry.Section.Number = i + 1;  // TODO: Maybe unnecessary
        auxEntry.Section.Selection = 0;
    }

    for (const FuncInfo &cur : funcs) {  // Symbols for functions
        const Symbol *symbol = cur.getSymbol();
        assert(symbol);
        if (!symbol->isUsed())
            continue;

        assert(symbol->getType() == Symbol::T_FUNCTION);

        if (symbolTable.append()) ERR_(R_BADMEMORY);
        IMAGE_SYMBOL &symbolEntry = symbolTable[-1];

        unsigned fNameLen = symbol->getFunctionNameLength();

        if (fNameLen <= IMAGE_SIZEOF_SHORT_NAME) {
            *(uint64_t *)&symbolEntry.N.ShortName = 0ull;
            memcpy((char *)symbolEntry.N.ShortName, symbol->getFunctionName(), fNameLen);
        } else {
            strings.extend(fNameLen + 1);
            //strings[-1] = 0;
            memcpy(&strings[-fNameLen - 1], symbol->getFunctionName(), fNameLen);

            symbolEntry.N.LongName[0] = 0;
            symbolEntry.N.LongName[1] = strings.getSize() - fNameLen - 1 + 4;
        }

        symbolEntry.Type = IMAGE_SYM_DTYPE_FUNCTION << 4;
        symbolEntry.NumberOfAuxSymbols = 0;

        if (cur.checkImport()) {  // Externally defined
            symbolEntry.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
            symbolEntry.Value = 0;
            symbolEntry.SectionNumber = 0;
        } else if (cur.checkStatic()) {  // Static
            symbolEntry.StorageClass = IMAGE_SYM_CLASS_STATIC;
            if (funcAddrs.append({symbolTable.getSize() - 1, cur.getAddr()}))
                ERR_(R_BADMEMORY);
            symbolEntry.Value = -1u;
            symbolEntry.SectionNumber = SECT_TEXT;
        } else {  // External
            symbolEntry.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
            if (funcAddrs.append({symbolTable.getSize() - 1, cur.getAddr()}))
                ERR_(R_BADMEMORY);
            symbolEntry.Value = -1u;
            symbolEntry.SectionNumber = SECT_TEXT;
        }

        if (symbolLookup.set(symbol->getFunctionName(), fNameLen, symbolTable.getSize() - 1))
            ERR_(R_BADMEMORY);
    }

    std::sort(funcAddrs.begin(), funcAddrs.end());

    curPos = 0;
    instrIdx = 0;
    funcAddrIdx = 0;

    for (const Instruction &cur : code) {  // Symbols for labels
        if (cur.isRemoved())
            continue;

        if (funcAddrIdx < funcAddrs.getSize() && funcAddrs[funcAddrIdx].instrIdx == instrIdx) {
            symbolTable[funcAddrs[funcAddrIdx].symIdx].Value = curPos;

            funcAddrIdx++;
        }

        const Symbol *symbol = cur.getSymbolHere();
        assert(symbol);
        if (symbol->isUsed()) {
            assert(symbol->getType() == Symbol::T_LABEL);  // Only labels are defined in the code itself

            if (symbolTable.append()) ERR_(R_BADMEMORY);
            IMAGE_SYMBOL &symbolEntry = symbolTable[-1];

            char buf[IMAGE_SIZEOF_SHORT_NAME + 1] = "";
            if (symbol->composeLabelName(buf, IMAGE_SIZEOF_SHORT_NAME + 1)) {
                ERR("Too many labels, please encapsulate your code better");
                ERR_(R_BADSYMBOL);
            }

            if (symbolLookup.has(buf)) {
                ERR("Duplicate label");
                ERR_(R_BADSYMBOL);
            }

            if (symbolLookup.set(buf, symbolTable.getSize() - 1))
                ERR_(R_BADMEMORY);

            *(uint64_t *)&symbolEntry.N.ShortName = 0ull;
            memcpy(symbolEntry.N.ShortName, buf, IMAGE_SIZEOF_SHORT_NAME);
            symbolEntry.Value = curPos;
            symbolEntry.SectionNumber = SECT_TEXT;
            symbolEntry.Type = 0;
            symbolEntry.StorageClass = IMAGE_SYM_CLASS_LABEL;
            symbolEntry.NumberOfAuxSymbols = 0;
        }

        curPos += cur.getLength();
        instrIdx++;
    }

    for (const LabelAlias &cur : labelAliases) {  // Symbols for duplicate labels
        Symbol newSymbol{},
               oldSymbol{};

        unsigned oldSymIdx = 0;

        if (newSymbol.ctorLabel(cur.newIdx) || oldSymbol.ctorLabel(cur.existentIdx)) {
            newSymbol.dtor();
            oldSymbol.dtor();
            ERR_(R_BADMEMORY);
        }

        if (symbolTable.append()) {
            newSymbol.dtor();
            oldSymbol.dtor();
            ERR_(R_BADMEMORY);
        }
        IMAGE_SYMBOL &symbolEntry = symbolTable[-1];

        char bufNew[IMAGE_SIZEOF_SHORT_NAME + 1] = "";
        if (newSymbol.composeLabelName(bufNew, IMAGE_SIZEOF_SHORT_NAME + 1)) {
            ERR("Too many labels, please encapsulate your code better");
            newSymbol.dtor();
            oldSymbol.dtor();
            ERR_(R_BADSYMBOL);
        }

        char bufOld[IMAGE_SIZEOF_SHORT_NAME + 1] = "";
        if (oldSymbol.composeLabelName(bufOld, IMAGE_SIZEOF_SHORT_NAME + 1)) {
            ERR("Too many labels, please encapsulate your code better");
            newSymbol.dtor();
            oldSymbol.dtor();
            ERR_(R_BADSYMBOL);
        }

        newSymbol.dtor();
        oldSymbol.dtor();

        if (symbolLookup.has(bufNew)) {
            ERR("Duplicate label");
            ERR_(R_BADSYMBOL);
        }

        if (symbolLookup.get(bufOld, &oldSymIdx)) {
            ERR("Alias for a non-existent label");
            ERR_(R_BADSYMBOL);
        }

        if (symbolLookup.set(bufNew, symbolTable.getSize() - 1)) {
            ERR_(R_BADMEMORY);
        }

        symbolEntry = symbolTable[oldSymIdx];
        *(uint64_t *)&symbolEntry.N.ShortName = 0ull;
        memcpy(symbolEntry.N.ShortName, bufNew, IMAGE_SIZEOF_SHORT_NAME);
    }


    // TODO: Replace with an actual entrypoint, move to the standard library
    /*{
    if (symbolTable.append()) ERR_(R_BADMEMORY);
    IMAGE_SYMBOL &symbolTest = symbolTable[-1];
    *(uint64_t *)&symbolTest.N.ShortName = 0ull;
    strcpy((char *)symbolTest.N.ShortName, "test");
    symbolTest.Value = 0;
    symbolTest.SectionNumber = SECT_TEXT;
    symbolTest.Type = IMAGE_SYM_DTYPE_FUNCTION << 4;
    symbolTest.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
    symbolTest.NumberOfAuxSymbols = 0;
    }*/

    sectText->SizeOfRawData = curPos;
    curPos = 0;

    for (const Instruction &cur : code) {
        unsigned curLen = cur.getLength();

        if (textRawData.extend(curLen))  ERR_(R_BADMEMORY);

        PackedInstruction pi{};
        if (cur.compile(pi)) ERR_(R_BADINSTR);

        char *dest = &textRawData[-curLen];
        if (pi.compile(&dest, curLen))  ERR_(R_BADMEMORY);

        pi.dtor();

        const Symbol *symbol = cur.getDispSymbol();
        assert(symbol);

        unsigned ripOffset = 0;
        unsigned dispOffset = cur.getDispOffset(&ripOffset);

        if (generateReloc(symbol, textRelocs, textRawData.getSize() - curLen +
                          dispOffset, symbolLookup, ripOffset))
            ERR_(R_BADSYMBOL);
    }

    /*

    COFF Header
    Section Table
    Symbol Table
    String Table (DWORD size, data. Size includes itself)
    Relocations for .text
    Raw data of .text

    */

    if (symbolTable.getSize() > 0) {
        coffHeader.PointerToSymbolTable = sizeof(coffHeader) + sizeof(IMAGE_SECTION_HEADER) * sectionTable.getSize();
    } else {
        coffHeader.PointerToSymbolTable = 0;
    }

    coffHeader.NumberOfSymbols = symbolTable.getSize();

    if (textRelocs.getSize() > 0) {
        sectText->PointerToRelocations = coffHeader.PointerToSymbolTable +
            sizeof(IMAGE_SYMBOL) * symbolTable.getSize()+ sizeof(DWORD) + strings.getSize();
    } else {
        sectText->PointerToRelocations = 0;
    }

    if (textRelocs.getSize() >= 0xffff) {
        sectText->NumberOfRelocations = 0xffff;
        sectText->Characteristics |= IMAGE_SCN_LNK_NRELOC_OVFL;
    } else {
        sectText->NumberOfRelocations = textRelocs.getSize();
    }

    if (textRawData.getSize() > 0) {
        sectText->PointerToRawData = sectText->PointerToRelocations + sizeof(textRelocs[0]) * textRelocs.getSize();

        if (sectText->Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) {
            sectText->PointerToRawData += sizeof(IMAGE_RELOCATION);
        }
    } else {
        sectText->PointerToRawData = 0;
    }

    for (unsigned i = 0; i < sectionTable.getSize(); ++i) {
        const IMAGE_SECTION_HEADER &cur = sectionTable[i];
        IMAGE_AUX_SYMBOL &auxEntry = reinterpret_cast<IMAGE_AUX_SYMBOL &>(symbolTable[auxSymIdx[i]]);

        auxEntry.Section.Length = cur.SizeOfRawData;
        auxEntry.Section.NumberOfRelocations = cur.NumberOfRelocations;
        auxEntry.Section.NumberOfLinenumbers = cur.NumberOfLinenumbers;
    }


    WRITE_(&coffHeader, sizeof(coffHeader), 1);
    WRITE_(sectionTable.getBuf(), sizeof(IMAGE_SECTION_HEADER), sectionTable.getSize());
    WRITE_(symbolTable .getBuf(), sizeof(IMAGE_SYMBOL),          symbolTable.getSize());

    {
    DWORD stringTableSize = strings.getSize() + 4;
    WRITE_(&stringTableSize, sizeof(DWORD), 1);
    }

    WRITE_(strings.getBuf(),       sizeof(char),                     strings.getSize());

    if (sectText->Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) {
        IMAGE_RELOCATION relocCount{};

        relocCount.VirtualAddress = textRelocs.getSize();

        WRITE_(&relocCount, sizeof(IMAGE_RELOCATION), 1);
    }

    WRITE_(textRelocs  .getBuf(), sizeof(IMAGE_RELOCATION),       textRelocs.getSize());
    WRITE_(textRawData .getBuf(), sizeof(char),                  textRawData.getSize());

    return lastResult = R_OK;

err:
    sectionTable.dtor();
    symbolTable .dtor();
    strings     .dtor();
    textRelocs  .dtor();
    textRawData .dtor();
    auxSymIdx   .dtor();
    funcAddrs   .dtor();

    symbolLookup.dtor();

    return lastResult;

    #undef WRITE_
    #undef ERR_
}

}

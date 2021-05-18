#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <cstdio>
#include <immintrin.h>

#include "general.h"
#include "vector.h"
#include "opcode.h"
#include "opcode_enums.h"
#include "symbol.h"
#include "lexer.h"


namespace abel {

class Instruction;

#pragma pack(1)

// Had to remove that from the class because C++ can't handle forward declarations of nested classes properly
union modrm_t {
    uint8_t full : 8;

    struct {
        uint8_t rm  : 3;
        uint8_t reg : 3;
        uint8_t mod : 2;
    };
};

union sib_t {
    uint8_t full : 8;

    struct {
        uint8_t base  : 3;
        uint8_t index : 3;
        uint8_t scale : 2;
    };
};

union rex_t {
    uint8_t rex;
    uint32_t full : 32;

    struct {
        struct {
            uint8_t B     : 1;
            uint8_t X     : 1;
            uint8_t R     : 1;
            uint8_t W     : 1;
            uint8_t magic : 4;
        };

        uint8_t op[3];
    };
};

union vex2_t {
    uint32_t full : 24;

    struct {
        uint8_t op    : 8;

        uint8_t pp    : 2;
        uint8_t L     : 1;
        uint8_t vvvv  : 4;
        uint8_t R     : 1;
    };
};

union vex3_t {
    uint32_t full : 32;

    struct {
        uint8_t op : 8;

        uint8_t pp    : 2;
        uint8_t L     : 1;
        uint8_t vvvv  : 4;
        uint8_t W     : 1;

        uint8_t mmmmm : 5;
        uint8_t B     : 1;
        uint8_t X     : 1;
        uint8_t R     : 1;
    };
};

#pragma pack()

struct PackedInstruction {
    enum type_e {
        T_PLAIN = 0,
        T_REX   = 1,
        T_VEX2  = 2,
        T_VEX3  = 3,
    };

    struct {
        unsigned prefCnt    : 3;  /// 0, 1, 2, 3, 4 (Prefix count)
        type_e   type       : 2;  /// Plain, REX, VEX2, VEX3
        unsigned opcodeSize : 2;  /// 1, 2, 3 (Opcode size for REX)
        bool     disp       : 1;  /// (Is displacement present?)
        unsigned dispSize   : 2;  /// 0, 1, 2, 3 (If displacement is present, its size's logarithm)
        bool     imm        : 1;  /// (Is immediate present?)
        unsigned immSize    : 2;  /// 0, 1, 2, 3 (If immediate is present, its size's logarithm)
        bool     modrm      : 1;  /// (Is modrm byte present?)
        bool     sib        : 1;  /// (Is sib byte present?)

        inline unsigned getPrefCnt() const {
            assert(prefCnt <= 4);

            return prefCnt;
        }

        inline type_e getType() const {
            return type;
        }

        inline unsigned getOpcodeSize() const {
            assert(opcodeSize);

            return getType() <= T_REX ? opcodeSize : 1;
        }

        inline bool hasDisp() const {
            return disp;
        }

        inline unsigned getDispSize() const {
            return hasDisp() ? dispSize : -1u;
        }

        inline bool hasImm() const {
            return imm;
        }

        inline unsigned getImmSize() const {
            return hasImm() ? immSize : -1u;
        }

        inline bool hasModrm() const {
            return modrm;
        }

        inline bool hasSib() const {
            assert(modrm || !sib);

            return sib;
        }

        inline void setPrefCnt(unsigned new_prefCnt) {
            assert(new_prefCnt <= 4);

            prefCnt = new_prefCnt;
        }

        inline void setType(type_e new_type) {
            type = new_type;
        }

        inline void setOpcodeSize(unsigned new_opcodeSize) {
            assert(getType() <= T_REX);
            assert(new_opcodeSize);

            opcodeSize = new_opcodeSize;
        }

        inline void setDispSize(unsigned new_dispSize) {
            assert(new_dispSize < 4);

            disp = new_dispSize != -1u;
            dispSize = new_dispSize;
        }

        inline void setImmSize(unsigned new_immSize) {
            assert(new_immSize < 4);

            imm = new_immSize != -1u;
            immSize = new_immSize;
        }

        inline void setHasModrm(bool hasModrm) {
            modrm = hasModrm;
        }

        inline void setHasSib(bool hasSib) {
            assert(modrm || !hasSib);

            sib = hasSib;
        }
    } flags;

    //--------------------------------------------------------------------------------

    uint8_t prefixes[4];

    union {
        uint8_t rawOp[3];
        rex_t   rex;
        vex2_t  vex2;
        vex3_t  vex3;
    };

    modrm_t modrm;
    sib_t sib;

    uint64_t displacement;
    uint64_t immediate;

    //--------------------------------------------------------------------------------

    bool setPrefixes(const uint8_t new_prefixes[4]);  // The unused ones have to be null

    bool setRawOp(const uint8_t new_op[3], unsigned opcodeSize);

    bool setRexOp(const uint8_t new_op[3], unsigned opcodeSize);

    bool setRawOpVariant(reg_e reg);

    bool setRexOpVariant(reg_e reg);

    //--------------------------------------------------------------------------------

    FACTORIES(PackedInstruction)

    bool ctor();
    void dtor();

    void hexDump() const;

    unsigned getLength() const;

    bool compile(char **dest, unsigned limit) const;

};


class FuncInfo {
public:
    FACTORIES(FuncInfo)

    bool ctor();

    void dtor();

    inline bool isUsed() const {
        return symbol.isUsed();
    }

    bool setFunction(const char *name, unsigned length);

    bool setExpFunction(const char *name, unsigned length, unsigned new_addr);

    bool setImpFunction(const char *name, unsigned length);

    inline void setExport(unsigned new_addr) {
        addr = new_addr;
        isImport = false;
    }

    inline void setImport() {
        addr = 0;
        isImport = true;
    }

    inline void setStatic(bool new_isStatic) {
        isStatic = new_isStatic;
    }

    inline bool checkImport() const {
        return isImport;
    }

    inline bool checkStatic() const {
        return isStatic;
    }

    inline unsigned getAddr() const {
        return isImport ? -1u : addr;
    }

    inline Symbol *getSymbol() {
        return &symbol;
    }

    inline const Symbol *getSymbol() const {
        return &symbol;
    }

private:
    Symbol symbol;
    bool isImport;
    bool isStatic;
    unsigned addr;
};


class ObjectFactory {
public:

    struct LabelAlias {
        unsigned newIdx;
        unsigned existentIdx;
    };

    static constexpr unsigned REGSTK_SIZE = 8;
    static constexpr reg_e REGSTK_REGS[REGSTK_SIZE]  = {
        REG_10, REG_11, REG_12, REG_13, REG_14, REG_15, REG_SI, REG_DI,
    };

    #undef R_OK

    enum result_e {
        R_OK = 0,
        R_BADMEMORY,
        R_BADPTR,
        R_BADSIZE,
        R_BADIO,
        R_BADSYMBOL,
        R_BADINSTR,
        R_BADCONTRACT,
        R_NOTIMPL,
        // TODO
    };

    FACTORIES(ObjectFactory)

    result_e ctor();

    void dtor();

    inline result_e getLastResult() const {
        return lastResult;
    }

    //--------------------------------------------------------------------------------

    void stkReset();

    reg_e stkTos(unsigned depth = 1) const;

    result_e stkPush();

    result_e stkPop();

    /// Move the stack's contents to the memory, leaving at most `except` behind
    result_e stkFlush();

    /// Same as flush, but maintains the tos in the zeroth register as the return value
    result_e stkFlushExceptOne();

    /// Move the stack's contents to the registers, ensuring at least `req` being pulled
    result_e stkPull(unsigned req = 0);

    /// Increase the memory stack alignment counter
    void stkPushMem(unsigned count = 1);

    /// Decrease the memory stack alignment counter
    void stkPopMem(unsigned count = 1);

    /// If necessary, increase the stack pointer to ensure 16-byte alignment after nArgs args are pushed
    result_e stkAlign(unsigned frameSize, unsigned nArgs);

    /// Revert the last stkAlign
    result_e stkUnalign();

    inline bool stkIsAligned() const {
        return !(stkCurMem & 0b1);  // Same as modulo division, but handles negativeness correctly
    }

    inline bool stkRegIsEmpty() const {
        return stkCurSize == 0;
    }

    //--------------------------------------------------------------------------------

    unsigned reserveLabel();

    /// Returns -1u on failure
    unsigned placeLabel();

    result_e placeLabel(unsigned reservedLabelIdx);

    /// Creates a function symbol at the next added instruction
    result_e defineFunction(const Token *name, bool mangle = true, bool isExport = true);

    /// Creates a function symbol at the next added instruction
    result_e defineFunction(const char *name, unsigned length, bool mangle = false, bool isExport = true);

    /// Requests a function from the outside, so that it can be referenced in the code
    result_e importFunction(const Token *name, bool mangle = true);

    /// Requests a function from the outside, so that it can be referenced in the code
    result_e importFunction(const char *name, unsigned length, bool mangle = false);

    result_e addInstr();

    Instruction &getLastInstr();

    //--------------------------------------------------------------------------------

    result_e compile(FILE *ofile) const;

    void dump() const;

private:
    mutable result_e lastResult;

    Vector<Instruction> code;
    Vector<FuncInfo> funcs;
    Vector<LabelAlias> labelAliases;

    unsigned nextLabelIdx;
    Symbol scheduledLabel;

    unsigned stkCurTos;     /// TOS 0 (first free cell) register index
    unsigned stkCurSize;    /// Size of the register part of the stack
    int      stkCurMem;     /// Size of the memory part of the stack (might be negative)
    bool     stkWasAligned; /// Whether the stack was aligned and needs to be reverted

    mutable bool bypass;

};

}


#endif // OBJECT_H

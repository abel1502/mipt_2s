#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <cstdio>
#include <immintrin.h>

#include "general.h"
#include "vector.h"
#include "opcode.h"


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

    //--------------------------------------------------------------------------------

    FACTORIES(PackedInstruction)

    bool ctor();
    void dtor();

    void hexDump() const;

    unsigned getLength() const;

    bool compile(char **dest) const;

};


class ObjectFactory {
public:
    #undef R_OK

    enum result_e {
        R_OK = 0,
        R_BADMEMORY,
        R_BADPTR,
        R_BADSIZE,
        R_NOTIMPL,
        // TODO
    };

    FACTORIES(ObjectFactory)

    result_e ctor();

    void dtor();

    //--------------------------------------------------------------------------------

    unsigned reserveLabel();

    /// Returns -1u on failure
    unsigned placeLabel();

    bool placeLabel(unsigned reservedLabelIdx);

    // TODO: Ways to reference a label in an instruction

    //--------------------------------------------------------------------------------

    result_e compile(FILE *ofile) const;

private:
    Vector<Instruction> code;

    unsigned nextLabelIdx;

};

}


#endif // OBJECT_H

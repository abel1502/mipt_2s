#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <cstdio>
#include <immintrin.h>

#include "general.h"
#include "vector.h"


namespace abel {


struct Instruction {
    #pragma pack(1)

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
        uint32_t full : 32;

        struct {
            uint8_t op3   : 8;
            uint8_t op2   : 8;
            uint8_t op1   : 8;

            uint8_t B     : 1;
            uint8_t X     : 1;
            uint8_t R     : 1;
            uint8_t W     : 1;
            uint8_t magic : 4;
        };
    };

    union vex2_t {
        uint32_t full : 24;

        struct {
            uint8_t op   : 8;

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

    //--------------------------------------------------------------------------------

    enum type_e {
        T_PLAIN = 0,
        T_REX   = 1,
        T_VEX2  = 2,
        T_VEX3  = 3,
    };

    enum reg_e {
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

    enum regSize_e {
        REGSIZE_B,
        REGSIZE_W,
        REGSIZE_D,
        REGSIZE_Q,
        REGSIZE_
    };

    struct {
        unsigned prefCnt    : 3;  /// 0, 1, 2, 3, 4 (Prefix count)
        type_e   type       : 2;  /// Plain, REX, VEX2, VEX3
        unsigned opcodeSize : 2;  /// 1, 2, 3 (Opcode size for REX)
        bool     displ      : 1;  /// (Is displacement present?)
        unsigned displSize  : 2;  /// 1, 2, 4, 8 (If displacement is present, its size)
        bool     imm        : 1;  /// (Is immediate present?)
        unsigned immSize    : 2;  /// 1, 2, 4, 8 (If immediate is present, its size)

        inline unsigned getPrefCnt() const {
            assert(prefCnt <= 4);

            return prefCnt;
        }

        inline type_e getType() const {
            return type;
        }

        inline unsigned getOpcodeSize() const {
            assert(opSize);

            return getType() <= T_REX ? opSize : 1;
        }

        inline bool hasDispl() const {
            return displ;
        }

        inline unsigned getDisplSize() const {
            return hasDispl() ? 0 : 1 << displSize;
        }

        inline bool hasImm() const {
            return imm;
        }

        inline unsigned getImmSize() const {
            return hasImm() ? 0 : immSize;
        }

        inline void setPrefCnt(unsigned new_prefCnt) {
            assert(new_prefCnt <= 4);

            prefCnt = new_prefCnt;
        }

        inline void setType(type_e new_type) {
            type = new_type;
        }

        inline void setOpcodeSize(unsigned new_opSize) {
            assert(getType() <= T_REX);
            assert(new_opSize);

            opSize = new_opSize;
        }

        inline void setDisplSize(unsigned new_displSize) {
            assert((new_displSize & (new_displSize - 1)) == 0);

            displ = new_displSize == 0;
            displSize = _tzcnt_u32(new_displSize);
        }

        inline void setImmSize(unsigned new_immSize) {
            assert((new_immSize & (new_immSize - 1)) == 0);

            imm = new_immSize == 0;
            immSize = _tzcnt_u32(new_immSize);
        }
    } flags;

    //--------------------------------------------------------------------------------

    uint8_t prefixes[4];

    union {
        rex_t  rex;
        vex2_t vex2;
        vex3_t vex3;
    };

    modrm_t modrm;
    sib_t sib;

    uint64_t displacement;
    uint64_t immediate;

    //--------------------------------------------------------------------------------

    inline void setModrmRm()

    //--------------------------------------------------------------------------------

    FACTORIES(Instruction)

    bool ctor();
    bool dtor();

    bool ctorPlain();
    bool ctorRex();
    bool ctorVex2();
    bool ctorVex3();

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



    result_e compile(FILE *ofile) const;

private:
    Vector<Instruction> code;

};


}


#endif // OBJECT_H

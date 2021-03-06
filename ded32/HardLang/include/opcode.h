#ifndef OPCODE_H
#define OPCODE_H

#include "general.h"
#include "object.h"
#include "opcode_enums.h"
#include "symbol.h"


namespace abel {

struct PackedInstruction;
union modrm_t;
union sib_t;
union rex_t;
union vex2_t;
union vex3_t;

class Instruction {
public:
    struct mode_t {
        enum mode_e {
            #undef MODE_REG
            #undef MODE_MEM_REG
            #undef MODE_MEM_RIP
            #undef MODE_MEM_SIB

            MODE_REG,
            MODE_MEM_REG,
            MODE_MEM_RIP,
            MODE_MEM_SIB,
        };

        enum sib_e {
            #undef SIB_BASE
            #undef SIB_INDEX

            SIB_BASE  = 0b01,
            SIB_INDEX = 0b10,
        };

        enum disp_e {
            #undef DISP_NONE
            #undef DISP_8
            #undef DISP_32

            DISP_NONE = 0b00,
            DISP_8    = 0b01,
            DISP_32   = 0b10,
        };

        mode_e mode;
        unsigned sib;
        disp_e disp;
    };

    struct OperandImm {
        union {
            int64_t  val_q;
            uint64_t val_qu;
        };

        inline unsigned getLength(unsigned size) const {
            assert(size != SIZE_XMM);

            return size == -1u ? 0 : 1 << size;
        }
    };

    struct OperandDisp {
        union {
            int64_t  val_q;
            uint64_t val_qu;
        };

        Symbol symbol;

        inline unsigned getLength(unsigned size) const {
            assert(size != SIZE_XMM);

            return size == -1u ? 0 : 1 << size;
        }
    };

    struct OperandR {
        reg_e reg;

        inline bool usesRexReg(size_e rSize) const {
            return rSize != -1u &&
                   (reg >= REG_8 || (REG_SP <= reg && reg <= REG_DI && rSize == SIZE_B));
        }

        bool writeModRm(modrm_t &modrm) const;
        bool writeRex(rex_t &rex) const;

        inline constexpr unsigned getLength(unsigned) const {
            return 0;
        }
    };

    struct OperandRM {
        mode_t mode;

        reg_e reg;  // Also `base` when in SIB mode.
        reg_e index;
        scale_e scale;

        inline bool usesRexReg(size_e rmSize) const {
            return rmSize != -1u &&
                   (((reg >= REG_8 || (REG_SP <= reg && reg <= REG_DI && rmSize == SIZE_B)) &&
                     (mode.mode <= mode.MODE_MEM_REG || (mode.mode == mode.MODE_MEM_SIB && mode.sib & mode.SIB_BASE))) ||
                    ((index >= REG_8 || (REG_SP <= reg && reg <= REG_DI && rmSize == SIZE_B)) &&
                     (mode.mode == mode.MODE_MEM_SIB && mode.sib & mode.SIB_INDEX)));
        }

        bool writeModRm(modrm_t &modrm, sib_t &sib, bool &hasSib) const;
        bool writeRex(rex_t &rex) const;

        inline unsigned getLength(unsigned size) const {
            return size == -1u ? 0 : mode.mode != mode.MODE_MEM_SIB ? 1 : 2;
        }
    };

    //--------------------------------------------------------------------------------

    FACTORIES(Instruction)

    bool ctor();

    void dtor();


    Instruction &setOp(Opcode_e new_op) noexcept;

    /// RM reg
    Instruction &setRmReg(reg_e reg) noexcept;

    /// RM [reg]
    Instruction &setRmMemReg(reg_e reg, mode_t::disp_e dispMode = mode_t::DISP_NONE) noexcept;

    /// RM [$+disp32]
    Instruction &setRmMemRip() noexcept;

    /// RM [disp32]
    Instruction &setRmSib() noexcept;

    /// RM [base </ + disp8/ + disp32>]
    Instruction &setRmSib(reg_e base, mode_t::disp_e dispMode = mode_t::DISP_NONE) noexcept;

    /// RM [index * scale </ + disp8/ + disp32>]
    Instruction &setRmSib(reg_e index, scale_e scale, mode_t::disp_e dispMode = mode_t::DISP_NONE) noexcept;

    /// RM [base + index * scale </ + disp8/ + disp32>]
    Instruction &setRmSib(reg_e base, reg_e index, scale_e scale, mode_t::disp_e dispMode = mode_t::DISP_NONE) noexcept;

    Instruction &setR(reg_e reg) noexcept;

    Instruction &setDisp(int64_t value) noexcept;

    /*Instruction &setDispLabel(unsigned idx);

    Instruction &setDispFunc(const Token *name);

    Instruction &setDispFunc(const char *name, unsigned length = -1u);*/

    Instruction &setImm(int64_t value) noexcept;


    inline bool isRemoved() const {
        return removed;
    }

    inline void remove() {
        removed = true;
    }

    inline void restore() {
        removed = false;
    }

    unsigned getLength() const;

    unsigned getDispOffset(unsigned *ripOffset = nullptr) const;

    bool compile(PackedInstruction &pi) const;

    inline Symbol *getDispSymbol() {
        return &disp.symbol;
    }

    inline const Symbol *getDispSymbol() const {
        return &disp.symbol;
    }

    inline Symbol *getSymbolHere() {
        return &symbolHere;
    }

    inline const Symbol *getSymbolHere() const {
        return &symbolHere;
    }

    inline Opcode_e getOp() const {
        return op;
    }

    inline const OperandRM &getRm() const {
        return rm;
    }

    inline const OperandR &getR() const {
        return r;
    }

    inline const OperandDisp &getDisp() const {
        return disp;
    }

    inline const OperandImm &getImm() const {
        return imm;
    }

private:
    Opcode_e    op;

    OperandRM   rm;
    OperandR    r;
    OperandDisp disp;
    OperandImm  imm;

    bool removed;

    Symbol symbolHere;

    //--------------------------------------------------------------------------------

    Instruction &setRm(mode_t mode, reg_e reg, reg_e index, scale_e scale) noexcept;

    bool compile(PackedInstruction &pi, unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize,
                 unsigned variant, uint8_t prefixes[4], unsigned opSize, uint8_t opBytes[3]) const;

    unsigned getLength(unsigned prefSize, unsigned opSize, unsigned rmSize,
                       unsigned rSize, unsigned dispSize, unsigned immSize) const;

    unsigned getDispOffset(unsigned prefSize, unsigned *ripOffset, unsigned opSize,
                           unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize) const;

    bool needsRex(unsigned rmSize, unsigned rSize, unsigned dispSize, unsigned immSize) const;

};

}


#endif // OPCODE_H

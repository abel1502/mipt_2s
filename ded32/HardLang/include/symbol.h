#ifndef SYMBOL_H
#define SYMBOL_H

#include <cassert>

#include "general.h"
#include "lexer.h"


namespace abel {

// For now all symbols are to be treated as 32-bit rip-relative adresses
class Symbol {
public:
    enum type_e {
        T_NONE,   /// Means the symbol is not used
        T_LABEL,
        T_FUNCTION,
        T_SAVEDSTK,
    };

    static constexpr unsigned PREFIX_LEN = 3;
    static constexpr char PREFIX_FUNCTION[] = "_f@";
    static constexpr char PREFIX_LABEL[]    = "_l@";

    static constexpr char SYM_SAVEDSTK[]    = "_stkSave";
    static constexpr unsigned
                          SYM_SAVEDSTK_LEN  = sizeof(SYM_SAVEDSTK) - 1;

    static_assert(sizeof(PREFIX_FUNCTION) == PREFIX_LEN + 1);
    static_assert(sizeof(PREFIX_LABEL)    == PREFIX_LEN + 1);
    static_assert(SYM_SAVEDSTK_LEN        <= 8);

    FACTORIES(Symbol)

    inline bool ctor() {
        return ctorNone();
    }

    bool ctorNone();

    bool ctorLabel(unsigned new_idx);

    /// Adds the function prefix
    bool ctorFunction(const Token *new_name);

    /// Doesn't add any prefix
    bool ctorFunction(const char *new_name, unsigned new_length = -1u);

    bool ctorSavedStk();

    void dtor();

    static void swap(Symbol &a, Symbol &b);

    bool composeLabelName(char *buf, unsigned limit) const;

    inline bool isUsed() const {
        return type != T_NONE;
    }

    inline type_e getType() const {
        return type;
    }

    inline unsigned getLabelIdx() const {
        assert(type == T_LABEL);

        return idx;
    }

    inline unsigned getFunctionNameLength() const {
        assert(type == T_FUNCTION);

        return length;
    }

    inline const char *getFunctionName() const {
        assert(type == T_FUNCTION);

        return name;
    }

private:
    type_e type = T_NONE;

    union {
        // T_NONE

        // T_LABEL
        unsigned idx;

        // T_FUNCTION
        struct {
            unsigned length;
            char *name;

        // T_SAVEDSTK
        };
    };
};

}

#endif // SYMBOL_H

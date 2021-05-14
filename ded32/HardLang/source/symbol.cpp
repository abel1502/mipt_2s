#include "symbol.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>


namespace abel {

bool Symbol::ctorNone() {
    type = T_NONE;

    return false;
}

bool Symbol::ctorLabel(unsigned new_idx) {
    assert(type == T_NONE);

    if (type != T_NONE)
        return true;

    type = T_LABEL;
    idx = new_idx;

    return false;
}

bool Symbol::ctorFunction(const Token *new_name) {
    assert(new_name);

    assert(type == T_NONE);

    if (type != T_NONE)
        return true;

    type = T_FUNCTION;

    length = strnlen(new_name->getStr(), new_name->getLength());
    name = (char *)calloc(length + PREFIX_LEN + 1, sizeof(char));

    if (!name)
        return true;

    sprintf(name, "%s%.*s", PREFIX_FUNCTION, length, new_name->getStr());

    return false;
}

bool Symbol::ctorFunction(const char *new_name, unsigned new_length) {
    assert(new_name);

    assert(type == T_NONE);

    if (type != T_NONE)
        return true;

    type = T_FUNCTION;

    length = strnlen(new_name, new_length);
    name = (char *)calloc(length + 1, sizeof(char));

    if (!name)
        return true;

    memcpy(name, new_name, length);

    return false;
}

void Symbol::dtor() {
    if (type == T_FUNCTION) {
        free(name);
        name = nullptr;
    }

    type = T_NONE;
}

bool Symbol::composeLabelName(char *buf, unsigned limit) const {
    assert(type == T_LABEL);

    return (unsigned)snprintf(buf, limit, "%s%u", PREFIX_LABEL, idx) >= limit;
}

}
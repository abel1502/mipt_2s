/*

This demonstrates a problem with TXLib's assert on MSVC.
In debug configuration an assertion of compile-time constant false expression causes
the 'C2124 divide or mod by zero' build error

*/

#include <TXLib.h>


int main() {
    assert(false);

    return 0;
}
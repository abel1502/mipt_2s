#include <TXLib.h>
#include <cassert>

#include "mandelbrot.h"


int main(int argc, char **argv) {
    MandelbrotDisplay md{};

    if (argc == 2 && strcmp(argv[1], "-t") == 0) {
        md.perfCount();
    } else {
        md.renderLoop();
    }

    return 0;
}

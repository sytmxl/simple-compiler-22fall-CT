#include <sym.h>
#include "err.h"
void error(char errn) {
    err << line_no << " " << errn << endl;
}
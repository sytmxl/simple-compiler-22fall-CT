#include "sym.h"
#include "err.h"

void error(char errn) {
    if (setting) err_buffer.push_back(to_string(line_no + 1) + " " + errn);
    else if (peeking) {}
    else err << line_no + 1 << " " << errn << endl;
}

void sym2error(Symbol sym) {//error map for phasing
    if (sym == SEMICN) error('i');
    else if (sym == RPARENT) error('j');
    else if (sym == RBRACK) error('k');
}
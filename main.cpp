#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mips.h"
#include "opt.h"

int main() {
    new_tab();
    mips << ".data" << endl;
    CompUnit();
    set();
//    print_error();
    auto ite = quads.begin();
    insert_tab("str0", SymEntry(I_STR,1,1, 1));
    quads.insert(ite, 1, Quadruple(STR, "str0", "\\n", ""));
//    optmize();
    print_quad();
    print_mips();
    mips.close();
    out.close();

    return 0;
}


#ifndef CT_MIPS_H
#define CT_MIPS_H
#include "sym.h"
extern ofstream mips;
extern bool mips_on;
extern int base_offset;
void print_mips();
string t(int last=0);
#endif //CT_MIPS_H

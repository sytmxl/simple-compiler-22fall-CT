#include "sym.h"
#include "err.h"
enum Op {
    DEF,ASSI,DIRECT,
    FUNC, PARA, VAR, CON,
    _ADD, _SUB, _MUL, _DIV, _MOD,
    _AND, _OR, _EQL, _NEQ, _LSS, _LEQ, _GRE, _GEQ,
    PUSH, CALL, RET, GETRET,
    IN, OUT, STR,
    COMP, LABEL, J,
    BEQ, BNE, BZ, BNZ,
    TAB, PUSH_STACK, POP_STACK,PUSH_GP,POP_GP,
    ARR, GETARR, SAVEARR
};

typedef struct Quadruple {
    Op op;
    string x;
    string y;
    string z;
    Quadruple(Op op1, string x1, string y1, string z1) : op(op1), x(x1), y(y1), z(z1) {}
}Quadruple;

extern vector<Quadruple> quads;
extern int uninit;

void add_quad(Op op=DEF, string x="NULL", string y="NULL", string z="NULL");
void print_quad();
string new_str(string value);
Op sym2op(Symbol sym);
vector<string> split(const string& str, const string& delim);
string new_temp_var();
int to_int(string str);
int cal(Symbol op, string x, string y);
int is_digit(string str);
string mid_out(Quadruple quad);
#ifndef CT_MID_H
#define CT_MID_H

#endif //CT_MID_H

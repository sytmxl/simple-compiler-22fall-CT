#ifndef CT_ERR_H
#define CT_ERR_H
#include "sym.h"

enum IType {
    I_NULL,
    I_DEFAULT,
    I_FUNC,
    I_VAR,//SymEntry.dim tells if is array and its dim
    I_CONST,
};

enum DType {
    D_DEFAULT,
    D_VOID,
    D_INT,
};

//struct Var {
//    int dim=0;
//    DType dType=D_DEFAULT;
//};

struct SymEntry {
    IType iType=I_DEFAULT;
    int dim=0;
    DType dType=D_DEFAULT;//return value
    int addr=0;
//    vector<Var> param;
    vector<int> param;//dim of parameters(as only int)
};

struct Tab {
    map<string, SymEntry> map;
    struct Tab *parent=nullptr;
    bool local = false;
};

extern map<int, char> errors;

void error(char errn='X', int line_number=line_no);
void sym2error(Symbol sym);
void tab_init();
void new_tab(bool local=false);
void pop_tab();
void insert_tab(const string &id, const SymEntry &entry);
SymEntry search_tab(string id);
void print_tab();
void print_error();

//extern map<string, SymEntry> *rootTab, *preTab;

#endif //CT_ERR_H

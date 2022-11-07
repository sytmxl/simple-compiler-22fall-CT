#ifndef CT_ERR_H
#define CT_ERR_H
#include "sym.h"
//int uninit = -1365681;
enum IType {
    I_NULL,
    I_DEFAULT,
    I_FUNC,
    I_VAR,//SymEntry.dim tells if is array and its dim
    I_CONST,
    I_STR,
    I_TEMP,
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

typedef struct SymEntry {
    IType iType=I_DEFAULT;
    int dim=0;
    DType dType=D_DEFAULT;//return value
    int value = -1365681;
    int addr = -1365681;
    string reg="";
    bool global= false;
//    vector<Var> param;
    vector<int> param;//dim of parameters(as only int)

    SymEntry(IType iType1, int dim1, DType dType1, vector<int> param1)
        : iType(iType1), dim(dim1), dType(dType1), param(param1) {}
    SymEntry(IType iType1, int dim1)
            : iType(iType1), dim(dim1){}
    SymEntry(IType iType1, int dim1, int value1)
            : iType(iType1), dim(dim1), value(value1){}
    SymEntry(IType iType1, int dim1, int value1, int addr1)
            : iType(iType1), dim(dim1), value(value1), addr(addr1){}
    SymEntry()= default;
    SymEntry(IType iType1): iType(iType1){};
}SymEntry;

typedef struct Tab {
    map<string, SymEntry> tab;
    Tab *parent=nullptr;
    bool local = false;
    Tab (Tab *parent1, bool local1):parent(parent1), local(local1){}
}Tab;

extern map<int, char> errors;
extern bool debug;
extern Tab *preTab;
extern vector<Tab*> tab_flow;

void error(char errn='X', int line_number=line_no);
void sym2error(Symbol sym);
void tab_init();
void new_tab(bool local=false);
void pop_tab();
void insert_tab(const string &id, const SymEntry &entry, Tab *tab=preTab);
SymEntry* search_tab(string id);
SymEntry* search_pre_tab(string id);
void print_tab();
void print_error();
void break_point();

//extern map<string, SymEntry> *rootTab, *preTab;

#endif //CT_ERR_H

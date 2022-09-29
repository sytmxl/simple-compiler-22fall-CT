#include <sym.h>
#include "phas.h"

using namespace std;

extern Symbol cla; //pre sym
extern vector<string> syms, buffer;
extern vector<Symbol> classes;
extern bool is_end, setting;
extern ofstream out;

int main() {
    CompUnit();
    set();
}

void error() {
    cout << "###########ERROR###########" << endl;
}

void print(string str) {
    if (setting) buffer.push_back("<" + str + ">");
    else out << "<" + str + ">" << endl;
}

void if_not_error(Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    if (cla != sym1 && cla != sym2 && cla != sym3 && cla != sym4) error();
}

void next_not_error(Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    next_sym();
    if (cla != sym1 && cla != sym2 && cla != sym3 && cla != sym4) error();
}

void CompUnit() {   //from last to last(to prevent read one sym ahead), using peek to keep balance
    peek_sym(4);
    while (classes[0] == CONSTTK || (classes[0] == INTTK && classes[1] == IDENFR && classes[2] != LPARENT)) {
        Decl();
        peek_sym(4);
    }
    while (classes[0] == VOIDTK || (classes[0] == INTTK && classes[1] == IDENFR && classes[2] == LPARENT)) {
        FuncDef();
        peek_sym(2);
    }
    MainFuncDef();
    print("CompUnit");
}

void Decl() {
    peek_sym();
    if (classes[0] == CONSTTK) ConstDecl();
    else if (classes[0] == INTTK) VarDecl();
    else error();
//    print("Decl");
}

void FuncDef() {
    FuncType();
    next_not_error(IDENFR);
    next_not_error(LPARENT);
    peek_sym();
    if (classes[0] != RPARENT) FuncFParams();
    next_not_error(RPARENT);
    Block();
    print("FuncDef");
}

void FuncType() {
    next_not_error(VOIDTK, INTTK);
    print("FuncType");
}

void MainFuncDef() {
    next_not_error(INTTK);
    next_not_error(MAINTK);
    next_not_error(LPARENT);
    next_not_error(RPARENT);
    Block();
    print("MainFuncDef");
}

void ConstDecl() {
    next_not_error(CONSTTK);
    next_not_error(INTTK);
    ConstDef();
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
        ConstDef();
        peek_sym();
    }
    next_not_error(SEMICN);
    print("ConstDecl");
}

void ConstDef() {
    Ident();
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();//at [
        ConstExp();
        next_not_error(RBRACK);
        peek_sym();
    }
    peek_sym();
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        ConstInitVal();
    }
    print("ConstDef");
}

void VarDecl() {
    BType();
    VarDef();
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
        VarDef();
        peek_sym();
    }
    next_not_error(SEMICN);
    print("VarDecl");
}

void VarDef() {
    next_not_error(IDENFR); //if no
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();//at [
        ConstExp();
        next_not_error(RBRACK);
        peek_sym();
    }
    peek_sym();
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        InitVal();
    }
    print("VarDef");
}

void ConstInitVal() {//almost same as InitVal
    peek_sym();
    if (classes[0] == LBRACE) {
        next_sym();//at {
        peek_sym();
        if (classes[0] != RBRACE) {
            ConstInitVal();
            peek_sym();
            while (classes[0] == COMMA) {
                next_sym();
                ConstInitVal();
                peek_sym();
            }
            next_not_error(RBRACE);
        }
    } else ConstExp();
    print("ConstInitVal");
}

void InitVal() {//almost same as ConstInitVal
    peek_sym();
    if (classes[0] == LBRACE) {
        next_sym();//at {
        peek_sym();
        if (classes[0] != RBRACE) {
            InitVal();
            peek_sym();
            while (classes[0] == COMMA) {
                next_sym();
                InitVal();
                peek_sym();
            }
            next_not_error(RBRACE);
        }
    } else Exp();
    print("InitVal");
}

void FuncFParams() {
    FuncFParam();
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
        FuncFParam();
        peek_sym();
    }
    print("FuncFParams");
}

void FuncFParam() {
    BType();
    Ident();
    peek_sym();
    if (classes[0] == LBRACK) { //'[' ']' { '[' ConstExp ']' }
        next_sym();//at [
        next_not_error(RBRACK);
        peek_sym();
        while (classes[0] == LBRACK) {
            next_sym();
            ConstExp();
            next_not_error(RBRACK);
            peek_sym();
        }
    }
    print("FuncFParam");
}

void Block() {
    next_not_error(LBRACE);
    peek_sym();
    while (classes[0] != RBRACE && !is_end) {
        BlockItem();
        peek_sym();
    }
    next_not_error(RBRACE);
    print("Block");
}

void BlockItem() {
    peek_sym();
    if (classes[0] == CONSTTK || classes[0] == INTTK) Decl();
    else Stmt();
//    out << "<BlockItem>" <<  endl;
}

void Stmt() {
    peek_sym();
    set();
    if (classes[0] == IDENFR) {
        LVal();
        next_sym();//at = or should be reverted
        if (cla != ASSIGN) { //Exp';'
            revert();
            Exp();
            next_not_error(SEMICN);
        } else {
            set(true);
            peek_sym();//at =
            if (classes[0] == GETINTTK) { //LVal'='@'getint''('')'';'
                next_sym();//at getint
                next_not_error(LPARENT);
                next_not_error(RPARENT);
                next_not_error(SEMICN);
            } else {    //LVal '='@ Exp';'
                Exp();
                next_not_error(SEMICN);
            }
        }
    } else {
        set(true);
        if (classes[0] == SEMICN) {
            next_sym();
        } else if (classes[0] == IFTK) {   //'if''('Cond')'Stmt['else'Stmt]
            next_sym();
            next_not_error(LPARENT);
            Cond();
            next_not_error(RPARENT);
            Stmt();
            peek_sym();
            if (classes[0] == ELSETK) {
                next_sym();//at else
                Stmt();
            }
        } else if (classes[0] == WHILETK) {   //'while''('Cond')'Stmt
            next_sym();//at while
            next_not_error(LPARENT);
            Cond();
            next_not_error(RPARENT);
            Stmt();
        } else if (classes[0] == BREAKTK) {   //'break'';'
            next_sym();
            next_not_error(SEMICN);
        } else if (classes[0] == RETURNTK) {   //'return'[Exp]';'
            next_sym();//at return
            peek_sym();
            if (classes[0] != SEMICN) Exp();
            next_not_error(SEMICN);
        } else if (classes[0] == PRINTFTK) {   //'printf''('FormatString{','Exp}')'';'
            next_sym();
            next_not_error(LPARENT);
            FormatString();
            peek_sym();
            while (classes[0] == COMMA) {
                next_sym();//at ,
                Exp();
                peek_sym();
            }
            next_not_error(RPARENT);
            next_not_error(SEMICN);
        } else if (classes[0] == CONTINUETK) {   //'continue'';'
            next_sym();
            next_not_error(SEMICN);
        } else if (classes[0] == LBRACE) Block();
        else error();
    }
    print("Stmt");
}

void LVal() {//Ident{'['Exp']'}
    Ident();
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();
        Exp();
        next_not_error(RBRACK);
        peek_sym();
    }
    print("LVal");
}

void FuncRParams() {
    Exp();
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
        Exp();
        peek_sym();
    }
    print("FuncRParams");
}

void UnaryExp() {
    peek_sym(2);
    if (classes[0] == IDENFR && classes[1] == LPARENT) {    //Ident'('[FuncRParams]')'
        next_sym();//at Ident
        next_sym();//at (
        peek_sym();
        if (classes[0] != RPARENT) FuncRParams();
        next_not_error(RPARENT);
    } else if (classes[0] == PLUS || classes[0] == MINU || classes[0] == NOT) {
        UnaryOp();
        UnaryExp();
    } else PrimaryExp();
    print("UnaryExp");
}

void PrimaryExp() {
    peek_sym();
    if (classes[0] == IDENFR) LVal();
    else if (classes[0] == LPARENT) {
        next_sym();
        Exp();
        next_not_error(RPARENT);
    } else if (classes[0] == INTCON) Number();
    else error();
    print("PrimaryExp");
}

void exp_loop(void (*func1)(), void (*func2)(), Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    func1();
    peek_sym();
    while (classes[0] == sym1 || classes[0] == sym2 || classes[0] == sym3 || classes[0] == sym4) {
        func2();
        next_sym();
        func1();

        peek_sym();
    }
}
//exp_loop
void LOrExp() {
    exp_loop(LAndExp, LOrExp_print, OR);
    print("LOrExp");
}

void LAndExp() {
    exp_loop(EqExp, LAndExp_print, AND);
    print("LAndExp");
}

void EqExp() {
    exp_loop(RelExp, EqExp_print, EQL, NEQ);
    print("EqExp");
}

void RelExp() {
    exp_loop(AddExp, RelExp_print, LSS, LEQ, GRE, GEQ);
    print("RelExp");
}

void AddExp() {
    exp_loop(MulExp, AddExp_print, PLUS, MINU);
    print("AddExp");
}

void MulExp() {
    exp_loop(UnaryExp, MulExp_print, MULT, DIV, MOD);
    print("MulExp");
}
//offset output
void LOrExp_print() {
    print("LOrExp");
}

void LAndExp_print() {
    print("LAndExp");
}

void EqExp_print() {
    print("EqExp");
}

void RelExp_print() {
    print("RelExp");
}

void AddExp_print() {
    print("AddExp");
}

void MulExp_print() {
    print("MulExp");
}
//simple
void Exp() {
    AddExp();
    print("Exp");
}

void Cond() {
    LOrExp();
    print("Cond");
}

void FormatString() {
    next_not_error(STRCON);
//    print("FormatString");
}

void UnaryOp() {
    next_not_error(PLUS, MINU, NOT);
    print("UnaryOp");
}

void Number() {
    IntConst();
    print("Number");
}

void IntConst() {
    next_not_error(INTCON);
//    print("IntConst");
}

void BType() {
    next_not_error(INTTK);
//    print("BType");
}

void Ident() {//terminal
    next_not_error(IDENFR);
//    print("Ident");
}

void ConstExp() {
    AddExp();
    print("ConstExp");
}

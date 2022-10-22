#include "sym.h"
#include "phas.h"
#include "err.h"

int outside_str=0;//count exp number out of format string
int loop_depth=0;
int return_line_no=-1, printf_line_no=-1;
vector<int> func_line_nos;
SymEntry g_entry;

void next_not_error(Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void Decl();
void FuncDef();
void MainFuncDef();
void ConstDecl();
void VarDecl();
void BType();
void ConstDef();
void Ident();
void ConstExp();
void ConstInitVal();
void VarDef();
void InitVal();
void FuncType();
vector<int> FuncFParams();
int Block();
int FuncFParam();
int BlockItem();

int Stmt();

int LVal();
int Exp();
void Cond();
void FormatString();
void UnaryOp();
void Number();
void IntConst();
vector<int> FuncRParams();
int AddExp();
int MulExp();

int UnaryExp();

int PrimaryExp();
int LAndExp();
int EqExp();
int RelExp();
int LOrExp();
int exp_loop(int (*func1)(), void (*func2)(), Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void print(string str);

void AddExp_print();
void MulExp_print();
void LAndExp_print();
void EqExp_print();
void RelExp_print();
void LOrExp_print();

void print(string str) {
    if (setting) buffer.push_back("<" + str + ">");
    else out << "<" + str + ">" << endl;
}

void next_not_error(Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    peek_sym();
    if (classes[0] != sym1 && classes[0] != sym2 && classes[0] != sym3 && classes[0] != sym4) sym2error(sym1);
    else next_sym();
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
}

void FuncDef() {
    FuncType();
    DType dtype = cla == INTTK ? D_INT : D_VOID;
    next_not_error(IDENFR);
    if (search_tab(sym).iType != I_NULL) error('b');
    string name = sym;
    next_not_error(LPARENT);
    peek_sym();
    bool pop= false;
    vector<int> param;
    if (classes[0] != RPARENT) {
        new_tab(true);
        param = FuncFParams();
        pop = true;
    }
    next_not_error(RPARENT);
    int ret = Block();//-1:no return or 'return;';  >=0: dim of return
    if (dtype == D_VOID and ret >= 0) error('f', return_line_no);//report line_no of last return
    if (dtype == D_INT and ret == -1) error('g');
    if (pop) pop_tab();
    SymEntry entry = {.iType=I_FUNC, .dType=dtype, .param=param};
    insert_tab(name, entry);
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
    if (search_tab(sym).iType != I_NULL) error('b');
    string name = sym;
    int dim=0;
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();//at [
        ConstExp();
        next_not_error(RBRACK);
        dim++;
        peek_sym();
    }
    peek_sym();
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        ConstInitVal();
    }
    SymEntry entry = {.iType=I_CONST, .dim=dim};
    insert_tab(name, entry);
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
    Ident();
    if (search_tab(sym).iType != I_NULL) error('b');
    string name = sym;
    int dim=0;
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();//at [
        ConstExp();
        next_not_error(RBRACK);
        dim++;
        peek_sym();
    }
    peek_sym();
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        InitVal();
    }
    SymEntry entry = {.iType=I_VAR, .dim=dim};
    insert_tab(name, entry);
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

vector<int> FuncFParams() {
    vector<int> param;
    param.push_back(FuncFParam());
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
        param.push_back(FuncFParam());
        peek_sym();
    }
    print("FuncFParams");
    return param;
}

int FuncFParam() {
    BType();
    Ident();
    peek_sym();
    int dim = 0;
    string name = sym;
    if (classes[0] == LBRACK) { //'[' ']' { '[' ConstExp ']' }
        next_sym();//at [
        next_not_error(RBRACK);
        dim++;
        peek_sym();
        while (classes[0] == LBRACK) {
            next_sym();
            ConstExp();
            next_not_error(RBRACK);
            dim++;
            peek_sym();
        }
    }
    SymEntry entry = {.iType=I_VAR, .dim=dim};
    insert_tab(name, entry);
    print("FuncFParam");
    return dim;
}

int Block() {
    int ret=-1;
    next_not_error(LBRACE);
    peek_sym();
    new_tab();
    while (classes[0] != RBRACE && !is_end) {
        int temp = BlockItem();
        ret = temp > ret? temp : ret;
        peek_sym();
    }
    next_not_error(RBRACE);
    pop_tab();
    print("Block");
    return ret;
}

int BlockItem() {
    peek_sym();
    if (classes[0] == CONSTTK || classes[0] == INTTK) Decl();
    else return Stmt();
    return -1;
}

int Stmt() {
    int ret=-1;
    peek_sym();
    set();
    if (classes[0] == IDENFR) {
        LVal();
        peek_sym();
        if (classes[0] == ASSIGN and g_entry.iType==I_CONST) error('h');
        next_sym();//at = or should be reverted
        if (cla != ASSIGN) { //Exp';'
            revert();
            Exp();
            next_not_error(SEMICN);
        } else {
            set(true);
            peek_sym();//at =
            if (classes[0] == GETINTTK) { //LVal'=''getint''('')'';'
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
            loop_depth++;
            Stmt();
            loop_depth--;
        } else if (classes[0] == BREAKTK) {   //'break'';'
            next_sym();
            if (loop_depth<=0) error('m');
            next_not_error(SEMICN);
        } else if (classes[0] == RETURNTK) {   //'return'[Exp]';'
            next_sym();//at return
            return_line_no = line_no;
            peek_sym();
            if (classes[0] != SEMICN) ret = Exp();
            next_not_error(SEMICN);
        } else if (classes[0] == PRINTFTK) {   //'printf''('FormatString{','Exp}')'';'
            next_sym();
            printf_line_no = line_no;
            next_not_error(LPARENT);
            FormatString();
            peek_sym();
            outside_str=0;
            while (classes[0] == COMMA) {
                next_sym();//at ,
                Exp();
                outside_str++;
                peek_sym();
            }
            if (inside_str != outside_str) error('l', printf_line_no);
            next_not_error(RPARENT);
            next_not_error(SEMICN);
        } else if (classes[0] == CONTINUETK) {   //'continue'';'
            next_sym();
            if (loop_depth<=0) error('m');
            next_not_error(SEMICN);
        } else if (classes[0] == LBRACE) Block();
        else if (classes[0] == LPARENT or classes[0] == INTCON){
            Exp();
            next_not_error(SEMICN);
        } else error();
    }
    print("Stmt");
    return ret;
}

int LVal() {//Ident{'['Exp']'}
    Ident();
    int dim=0;
    g_entry = search_tab(sym);
    if (g_entry.iType==I_NULL) error('c');
    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();
        Exp();
        dim++;
        next_not_error(RBRACK);
        peek_sym();
    }
    print("LVal");
    return g_entry.dim - dim;
}

vector<int> FuncRParams() {
    vector<int> param;
    param.push_back(Exp());
    peek_sym();
    int num=1;
    while (classes[0] == COMMA) {
        next_sym();
        param.push_back(Exp());
        num++;
        peek_sym();
    }
    print("FuncRParams");
    return param;
}

int UnaryExp() {
    peek_sym(2);
    int dim=0;
    if (classes[0] == IDENFR && classes[1] == LPARENT) {    //Ident'('[FuncRParams]')'
        next_sym();//at Ident
        func_line_nos.push_back(line_no);
        SymEntry entry = search_tab(sym);
        if (entry.iType == I_NULL) error('c');
        dim = entry.dType == D_VOID? -1 : 1;
        next_sym();//at (
        peek_sym();
        if (classes[0] != RPARENT) {
            vector<int> param = FuncRParams();
            if (entry.param.size() != param.size()) error('d',func_line_nos.back());
            for (int i=0; i < param.size(); i++)
                if (param[i] != entry.param[i]) error('e', func_line_nos.back());
        }
        next_not_error(RPARENT);
        func_line_nos.pop_back();
    } else if (classes[0] == PLUS || classes[0] == MINU || classes[0] == NOT) {
        UnaryOp();
        dim = UnaryExp();
    } else dim = PrimaryExp();
    print("UnaryExp");
    return dim;
}

int PrimaryExp() {
    peek_sym();
    int dim=0;
    if (classes[0] == IDENFR) dim = LVal();
    else if (classes[0] == LPARENT) {
        next_sym();
        dim = Exp();
        next_not_error(RPARENT);
    } else if (classes[0] == INTCON) Number();
    else error();
    print("PrimaryExp");
    return dim;
}

int exp_loop(int (*func1)(), void (*func2)(), Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    int dim=0;
    int ret = func1();
    if (ret > dim) dim = ret;
    peek_sym();
    while (classes[0] == sym1 || classes[0] == sym2 || classes[0] == sym3 || classes[0] == sym4) {
        func2();
        next_sym();
        int ret = func1();
        if (ret > dim) dim = ret;
        peek_sym();
    }
    func2();
    return ret;
}
//exp_loop
int LOrExp() {
    return exp_loop(LAndExp, LOrExp_print, OR);
}

int LAndExp() {
    return exp_loop(EqExp, LAndExp_print, AND);
}

int EqExp() {
    return exp_loop(RelExp, EqExp_print, EQL, NEQ);
}

int RelExp() {
    return exp_loop(AddExp, RelExp_print, LSS, LEQ, GRE, GEQ);
}

int AddExp() {
    return exp_loop(MulExp, AddExp_print, PLUS, MINU);
}

int MulExp() {
    return exp_loop(UnaryExp, MulExp_print, MULT, DIV, MOD);
}
//offset output
void LOrExp_print() { print("LOrExp");}
void LAndExp_print() {print("LAndExp");}
void EqExp_print() {print("EqExp");}
void RelExp_print() {print("RelExp");}
void AddExp_print() {print("AddExp");}
void MulExp_print() {print("MulExp");}
//simple
int Exp() {
    int dim = AddExp();
    print("Exp");
    return dim;
}

void Cond() {
    LOrExp();
    print("Cond");
}

void FormatString() {
    next_not_error(STRCON);
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
}

void BType() {
    next_not_error(INTTK);
}

void Ident() {//terminal
    next_not_error(IDENFR);
    if (sym.compare("ok") == 0) print_tab();
}

void ConstExp() {
    AddExp();
    print("ConstExp");
}

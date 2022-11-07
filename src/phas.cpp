#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mid.h"

int outside_str=0;//count exp number out of format string
int loop_depth=0;
int return_line_no=-1, printf_line_no=-1;
vector<int> func_line_nos;
SymEntry *g_entry;
bool defining = false, //if check is duplicated
    global = true,
    cons = false;//if fully cal when defining const
bool open_cal = true;
bool push = true;

void next_not_error(Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void Decl();
void FuncDef();
void MainFuncDef();
void ConstDecl();
void VarDecl();
void BType();
void ConstDef();
void Ident();
void VarDef();
string ConstInitVal();
string InitVal();
void FuncType();
vector<int> FuncFParams();
int Block();
int FuncFParam();
int BlockItem();
int Stmt();

string LVal();

void Cond();
void FormatString();
void UnaryOp();
string Number();
string IntConst();
vector<int> FuncRParams();
string ConstExp();
string Exp();
string AddExp();
string MulExp();

string UnaryExp();//function call

string PrimaryExp();
string LAndExp();
string EqExp();
string RelExp();
string LOrExp();

string exp_loop(string (*func)(), void (*print_func)(), Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);

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
    global = false;
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
    defining = true;
    FuncType();
    DType dtype = cla == INTTK ? D_INT : D_VOID;
    Ident();
    defining = false;
    if (search_tab(sym)->iType != I_NULL) error('b');
    string name = sym;
    add_quad(FUNC, dtype == D_INT ? "int" : "void", name);
    next_not_error(LPARENT);
    peek_sym();
    vector<int> param;
    new_tab(true);
    if (classes[0] != RPARENT)
        param = FuncFParams();
    next_not_error(RPARENT);
    SymEntry entry(I_FUNC, 0, dtype, param);
    insert_tab(name, entry, preTab->parent);//insert to last tab(root tab)
    int ret = Block();//-1:no return or 'return;';  >=0: dim of return
    if (dtype == D_VOID and ret >= 0) error('f', return_line_no);//report line_no of last return
    if (dtype == D_INT and ret == -1) error('g');
    if (dtype == D_VOID) add_quad(RET, "$v0");
    pop_tab();
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
    add_quad(FUNC, "int", "main");
    int ret = Block();//-1:no return or 'return;';  >=0: dim of return
    if (ret == -1) error('g');
    print("MainFuncDef");
}

void ConstDecl() {
    cons = true;
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
    cons = false;
}

void ConstDef() {
    defining = true;
    Ident();
    defining = false;
    if (search_tab(sym)->iType != I_NULL) error('b');
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
    int value=uninit;
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        value= to_int(ConstInitVal());
        add_quad(CON, name, to_string(value));
//        add_quad(CON, name, "$t1");
    } else add_quad(CON, name);
    SymEntry entry(I_CONST, dim, value);
    entry.global = global;
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
    defining = true;
    Ident();
    defining = false;
    if (search_tab(sym)->iType != I_NULL) error('b');
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
    int value=uninit;
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        value = to_int(InitVal());
//        add_quad(VAR, name, InitVal());//TODO
        add_quad(VAR, name, "$t1");
    } else add_quad(VAR, name);
    SymEntry entry(I_VAR, dim, value);
    entry.global = global;
    insert_tab(name, entry);
    print("VarDef");
}

string ConstInitVal() {//almost same as InitVal
    peek_sym();
    string ret;
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
        }
        next_not_error(RBRACE);
    } else ret = ConstExp();
    print("ConstInitVal");
    return ret;
}

string InitVal() {//almost same as ConstInitVal
    peek_sym();
    string ret;
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
        }
        next_not_error(RBRACE);
    } else ret = Exp();
    print("InitVal");
    return ret;
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
    if (search_tab(sym)->iType != I_NULL) error('b');
    peek_sym();
    int dim = 0;
    string name = sym;
    add_quad(PARA, name);
    if (classes[0] == LBRACK) { //'[' ']' { '[' ConstExp ']' }
        next_sym();//at [
        next_not_error(RBRACK);//TODO array
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
    SymEntry entry(I_VAR, dim);
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
    print_tab();//for view
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
//    int ret=-1;
    string ret = "NULL";
    peek_sym();
    set();
    if (classes[0] == IDENFR) {
        LVal();
        string name = sym;
        peek_sym();
        if (classes[0] == ASSIGN and g_entry->iType==I_CONST) error('h');
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
                add_quad(IN, name);
            } else {    //LVal '='@ Exp';'
                string exp = Exp();
                if (exp == "RET") add_quad(GETRET, name);
                else add_quad(ASSI, name, exp);
                next_not_error(SEMICN);
            }
        }
    } else {
        set(true);
        if (classes[0] == SEMICN) {     //;
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
            add_quad(RET, ret);
        } else if (classes[0] == PRINTFTK) {   //'printf''('FormatString{','Exp}')'';'
            next_sym();
            printf_line_no = line_no;
            next_not_error(LPARENT);
            FormatString();
            vector<string> strs = split(sym, "'\"");
            peek_sym();
            outside_str=0;
            int i = 0;
            while (i < strs.size()) {//print str before %d
                string str = strs[i];
                if (str == "#") add_quad(OUT, "str0");//\n
                else if (str == "$") break;
                else add_quad(OUT, new_str(str));
                i++;
            }
            while (classes[0] == COMMA) {
                next_sym();//at ,
                while (i < strs.size()) {//print str before %d
                    string str = strs[i++];
                    if (str == "#") add_quad(OUT, "str0");//\n
                    else if (str == "$") break;
                    else add_quad(OUT, new_str(str));
                }
                Exp();
                add_quad(OUT, "$t1");
                outside_str++;
                peek_sym();
            }
            while (i < strs.size()) {//print str before %d
                string str = strs[i];
                if (str == "#") add_quad(OUT, "str0");//\n
                else if (str == "$") break;
                else add_quad(OUT, new_str(str));
                i++;
            }
            if (inside_str != outside_str) error('l', printf_line_no);
            next_not_error(RPARENT);
            next_not_error(SEMICN);
        } else if (classes[0] == CONTINUETK) {   //'continue'';'
            next_sym();
            if (loop_depth<=0) error('m');
            next_not_error(SEMICN);
        } else if (classes[0] == LBRACE) Block();   //block
        else {//too much conditions, no error processing here so...
            Exp();
            next_not_error(SEMICN);
        }
    }
    print("Stmt");
//    return ret;
    return 1;
}

string LVal() {//Ident{'['Exp']'}
    Ident();
//    int dim=0;
    string ret = sym;
    g_entry = search_tab(sym);
    if(!defining) if (g_entry->iType==I_NULL) error('c');

    peek_sym();
    while (classes[0] == LBRACK) {
        next_sym();
        Exp();
//        dim++;
        next_not_error(RBRACK);
        peek_sym();
    }
    print("LVal");
    return ret;
    //can't use const array in funcRparam
//    if (g_entry.iType==I_CONST and g_entry.dim > 0 and g_entry.dim - dim > 0) return 3;
//    return g_entry.dim - dim;
}

vector<int> FuncRParams() {
    vector<int> param;
//    param.push_back(Exp());
    add_quad(PUSH, Exp());
    peek_sym();
    while (classes[0] == COMMA) {
        next_sym();
//        param.push_back(Exp());
        add_quad(PUSH, Exp());
        peek_sym();
    }
    print("FuncRParams");
    return param;
}

string UnaryExp() {
    peek_sym(2);
//    int dim=0;
    string ret;
    if (classes[0] == IDENFR && classes[1] == LPARENT) {    //Ident'('[FuncRParams]')'
        next_sym();//at Ident
        string name = sym;
        func_line_nos.push_back(line_no);
        SymEntry *entry = search_tab(sym);
//        if(!defining) if (entry.iType == I_NULL) {
//            error('c');
//            while (cla != RPARENT) next_sym();
//            return 3;
//        }
//        dim = entry.dType == D_VOID? 3 : 0;
        next_sym();//at (
        peek_sym();
        if (classes[0] != RPARENT and classes[0] != SEMICN) {
            vector<int> param = FuncRParams();
//            if (entry.param.size() != param.size()) error('d',func_line_nos.back());
//            else
//                for (int i=0; i < param.size(); i++)
//                    if (param[i] != entry.param[i]) error('e', func_line_nos.back());
        } else //func();
            if (!entry->param.empty()) error('d',func_line_nos.back());
        func_line_nos.pop_back();
        add_quad(CALL, name);
        ret = "RET";
        next_not_error(RPARENT);
    } else if (classes[0] == PLUS || classes[0] == MINU || classes[0] == NOT) {
        UnaryOp();
        push = false;
        if (cons) {
            if (cla == MINU) ret= "-";
            ret += UnaryExp();
        } else {
            ret = new_temp_var();
            add_quad(sym2op(cla), "0", UnaryExp(), ret);
        }
    } else ret = PrimaryExp();
    if (!cons) {
        add_quad(ASSI, "$t1", ret);

    }
    print("UnaryExp");
    return ret;
}

string PrimaryExp() {
    peek_sym();
//    int dim=0;
    string dim;
    if (classes[0] == IDENFR) dim = LVal();
    else if (classes[0] == LPARENT) {
        next_sym();
        dim = Exp();
        next_not_error(RPARENT);
    } else if (classes[0] == INTCON) dim = Number();
    else error();
    print("PrimaryExp");
    return dim;
}

string exp_loop(string (*func)(), void (*print_func)(), Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    vector<string> vars;
    string ret, temp;
    Symbol op=DEFAULT;
    int con = uninit;
    vector<Symbol> ops;
    temp = func();
    //add_quad(PUSH_STACK);
    if (open_cal and cons and (is_digit(temp)!=uninit or search_tab(temp)->iType==I_CONST or global))
        con = to_int(temp);
    else ret = temp;
    peek_sym();
    while (classes[0] == sym1 || classes[0] == sym2 || classes[0] == sym3 || classes[0] == sym4) {
        print_func();
        Symbol symbol = classes[0];//operator
        next_sym();
        add_quad(PUSH_STACK);
        temp = func();
        if (open_cal and cons and (is_digit(temp)!=uninit or search_tab(temp)->iType==I_CONST)) {
            if (con == uninit) {//no con before
                con = to_int(temp);
                op = symbol;
            }//has con before
            else con = cal(symbol, to_string(con), temp);//const cal here
        } else {
            add_quad(POP_STACK);
            add_quad(sym2op(symbol), "$t0", "$t1", "$t1");
            //add_quad(PUSH_STACK);
        }
        peek_sym();
    }
    if (ret.empty())
        ret = to_string(con);//con first, ret=con for cal below
    print_func();
    if (!cons) ret = "$t1";//return digit only at const def
    return ret;
}
//exp_loop
string LOrExp() {
    return exp_loop(LAndExp, LOrExp_print, OR);
}

string LAndExp() {
    return exp_loop(EqExp, LAndExp_print, AND);
}

string EqExp() {
    return exp_loop(RelExp, EqExp_print, EQL, NEQ);
}

string RelExp() {
    return exp_loop(AddExp, RelExp_print, LSS, LEQ, GRE, GEQ);
}

string AddExp() {
    return exp_loop(MulExp, AddExp_print, PLUS, MINU);
}

string MulExp() {
    push = true;
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
string Exp() {
//    int dim = AddExp();
    string name = AddExp();
    print("Exp");
    return name;
//    return dim;
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

string Number() {
    string ret = IntConst();
    print("Number");
    return ret;
}

string IntConst() {
    next_not_error(INTCON);
    return sym;
}

void BType() {
    next_not_error(INTTK);
}

void Ident() {//terminal
    next_not_error(IDENFR);
    if (sym.compare("ok") == 0) print_tab();
}

string ConstExp() {
    string ret = AddExp();
    print("ConstExp");
    return ret;
}

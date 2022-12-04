#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mid.h"
#include "mips.h"
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
vector<int> whiles;
int if_counter = 1, while_counter = 1, logic_counter = 1;
bool is_arr = false;
int arr_i = 0, arr_j = 0, arr_x = 0, arr_y = 0;
string arr_name;


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
string ConstExp_Arr();
string Exp();
string Exp_Arr();
string AddExp();
string MulExp();

string UnaryExp();//function call

string PrimaryExp();
string LAndExp();
string EqExp();
string RelExp();
string LOrExp();

string exp_loop(string (*func)(), void (*print_func)(), Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
string logic_loop(string (*func)(), void (*print_func)(), Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void print(string str);

void AddExp_print();
void MulExp_print();
void LAndExp_print();
void EqExp_print();
void RelExp_print();
void LOrExp_print();

string cal_offset(string name, int x, int y) {
    SymEntry *entry = search_tab(name);
    return to_string((y + x*entry->j)*4);
}

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
//    if (search_tab(sym)->iType != I_NULL) error('b');
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
    defining = true;
    Ident();
    defining = false;
//    if (search_tab(sym)->iType != I_NULL) error('b');
    string name = sym;
    int dim=0;
    peek_sym();
    arr_i = arr_j = -1;//init
    arr_x = arr_y = 0;
    while (classes[0] == LBRACK) {//1 dim arr a[x]->a[1][x]
        arr_name = name;
        next_sym();//at [
        is_arr = false;//exp here should not be arr init
        //i first
        if (arr_i == -1) arr_i = to_int(ConstExp());
        else arr_j = to_int(ConstExp());
        next_not_error(RBRACK);
        dim++;
        peek_sym();
        is_arr = true;//create a env for arr init
    }
    if (arr_j == -1) {//1 dim arr a[x]->a[1][x]
        arr_j = arr_i;
        arr_i = 1;
    }
    peek_sym();
    int value=uninit;
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        if (is_arr) {
            add_quad(ARR, name, to_string(arr_i), to_string(arr_j));
            ConstInitVal();
        } else {
            value = to_int(ConstInitVal());
            add_quad(CON, name, to_string(value));
        }
//        add_quad(CON, name, "$t1");
    } else if (is_arr) add_quad(ARR, name, to_string(arr_i), to_string(arr_j));
    else add_quad(CON, name);
    SymEntry entry(I_CONST, dim, value, arr_i, arr_j);
    entry.global = global;
    insert_tab(name, entry);
    print("ConstDef");
    is_arr = false;
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
//    if (search_tab(sym)->iType != I_NULL) error('b');
    string name = sym;
    int dim=0;
    peek_sym();
    arr_i = arr_j = -1;//init
    arr_x = arr_y = 0;
    while (classes[0] == LBRACK) {
        arr_name = name;
        next_sym();//at [
        is_arr = false;
        if (arr_i == -1) arr_i = to_int(ConstExp());
        else arr_j = to_int(ConstExp());
        next_not_error(RBRACK);
        dim++;
        peek_sym();
        is_arr = true;//create a env for arr init
    }
    if (arr_j == -1) {//1 dim arr a[x]->a[1][x]
        arr_j = arr_i;
        arr_i = 1;
    }
    peek_sym();
    int value=uninit;
    if (classes[0] == ASSIGN) {
        next_sym();//at =
        if (is_arr) {
            add_quad(ARR, name, to_string(arr_i), to_string(arr_j));
            InitVal();
        } else {
            value = to_int(InitVal());
            add_quad(VAR, name, "$t1");
        }
//        add_quad(CON, name, "$t1");
    } else if (is_arr) add_quad(ARR, name, to_string(arr_i), to_string(arr_j));
    else add_quad(VAR, name);
    SymEntry entry(I_VAR, dim, value, arr_i, arr_j);
    entry.global = global;
    insert_tab(name, entry);
    print("VarDef");
    is_arr = false;
}

string ConstInitVal() {//almost same as InitVal
    peek_sym();
    string ret=to_string(uninit);
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
    } else ret = ConstExp_Arr();
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
    } else ret = Exp_Arr();
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
//    if (search_tab(sym)->iType != I_NULL) error('b');
    peek_sym();
    int dim = 0;
    string name = sym;
    add_quad(PARA, name);
    if (classes[0] == LBRACK) { //'[' ']' { '[' ConstExp ']' }
        next_sym();//at [
        next_not_error(RBRACK);
        dim++;
        peek_sym();
        while (classes[0] == LBRACK) {
            next_sym();
            arr_j = to_int(ConstExp());
            next_not_error(RBRACK);
            dim++;
            peek_sym();
        }
    }
    SymEntry entry(I_PARA, dim, 0, arr_i, arr_j);

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
//    print_tab();//for view
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
        string lval = LVal();
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
                vector<string> strs = split(lval, "#");
                if (strs.size() > 1) {
                    add_quad(DIRECT, "move $t0, $t1");
                    add_quad(IN, "$t1");
                    add_quad(SAVEARR, strs[0], "$t0", "$t1");
                } else add_quad(IN, name);
            } else {    //LVal '='@ Exp';'
                vector<string> strs = split(lval, "#");
                if (strs.size() > 1) add_quad(PUSH_STACK);//$t1 = offset, will be flushed by exp, so save it
                string exp = Exp();
                if (exp == "RET") exp = "$v0";
                if (strs.size() > 1) {//arr = exp
                    add_quad(POP_STACK);
                    add_quad(SAVEARR, strs[0], "$t0", exp);
                } else {
                    add_quad(ASSI, name, exp);
                }
                next_not_error(SEMICN);
            }
        }
    } else {
        set(true);
        if (classes[0] == SEMICN) {     //;
            next_sym();
        } else if (classes[0] == IFTK) {   //'if''('Cond')'Stmt['else'Stmt]
            string n = to_string(if_counter++);
            add_quad(LABEL, "if_"+n);
            next_sym();
            next_not_error(LPARENT);
            Cond();
            add_quad(DIRECT, "beqz $t1, end_if_"+n);//goto else or end_if
            next_not_error(RPARENT);
            Stmt();
            peek_sym();
            if (classes[0] == ELSETK) {
                add_quad(DIRECT, "j end_else_"+n);
                add_quad(LABEL, "end_if_"+n);
                add_quad(LABEL, "else_"+n);
                next_sym();//at else
                Stmt();
                add_quad(LABEL, "end_else_"+n);
            } else add_quad(LABEL, "end_if_"+n);
            add_quad(LABEL, "end_if_else_"+n);
        } else if (classes[0] == WHILETK) {   //'while''('Cond')'Stmt
            int n = while_counter++;
            whiles.push_back(n);
            add_quad(LABEL, "while_"+to_string(n));
            next_sym();//at while
            next_not_error(LPARENT);
            Cond();
            add_quad(DIRECT, "blez $t1, end_while_"+to_string(n));
            next_not_error(RPARENT);
            loop_depth++;
            Stmt();
            loop_depth--;
            add_quad(DIRECT, "j while_"+to_string(n));
            add_quad(LABEL, "end_while_"+to_string(n));
            whiles.pop_back();
        } else if (classes[0] == BREAKTK) {   //'break'';'
            next_sym();
            int n = whiles.back();
//            whiles.pop_back();
            add_quad(DIRECT, "j end_while_"+to_string(n));
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
            while (classes[0] == COMMA) {
                next_sym();//at ,
                Exp();
                add_quad(PUSH_STACK);
                outside_str++;
                peek_sym();
            }
            int dig=outside_str-1;
            while (i < strs.size()) {
                string str = strs[i++];
                if (str == "#") add_quad(OUT, "str0");//\n
                else if (str == "$") {
                    add_quad(DIRECT, "lw $t1, "+ to_string(dig--*4) + "($sp)");
                    add_quad(OUT, "$t1");
                }
                else add_quad(OUT, new_str(str));
            }
            add_quad(DIRECT, "addi $sp,$sp,"+ to_string(4*outside_str));
            if (inside_str != outside_str) error('l', printf_line_no);
            next_not_error(RPARENT);
            next_not_error(SEMICN);
        } else if (classes[0] == CONTINUETK) {   //'continue'';'
            next_sym();
            int n = whiles.back();
//            whiles.pop_back();
            add_quad(DIRECT, "j while_"+to_string(n));
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
    SymEntry *entry = search_tab(sym);
//    if(!defining) if (g_entry->iType==I_NULL) error('c');
    peek_sym();
    int i=0;
    if (classes[0] == LBRACK) {//arr
        int x=-1,y=-1;
        //don't know if const[const][const], save data anyway
        while (classes[0] == LBRACK) {//save
            next_sym();
            int t = to_int(Exp());
            add_quad(PUSH_STACK);
            if (x == -1) x = t;
            else y = t;
            i++;
//        dim++;
            next_not_error(RBRACK);
            peek_sym();
        }
        if (x != uninit and y != uninit and entry->iType == I_CONST) {//const[const][const]
            if (i == 2) {//a[i][j]
                x = (x * entry->j + y) * 4;
            } else {//a[i]
                x *= 4;
            }
            while(i--) add_quad(POP_STACK);
            ret = ret + "#" + to_string(x);
            ret = to_string(to_int(ret));
        } else {//cal offset at runtime
            if (i == 2) {//a[i][j]
                add_quad(POP_STACK);
                add_quad(POP_STACK);//$t1 = y
                add_quad(_MUL, "$t0", to_string(entry->j), "$t0");//x*j
                add_quad(_ADD, "$t0", "$t1", "$t1");
                add_quad(_MUL, "$t1", to_string(4), "$t1");
            } else {//a[i]
                if (i < entry->dim) {//y[0] of y[1][2]
                    add_quad(POP_STACK);//$t1=$t0=x
                    add_quad(_MUL, "$t0", to_string(entry->j), "$t1");//x*j
                    add_quad(_MUL, "$t1", to_string(4), "$t1");
                } else {
                    add_quad(POP_STACK);//$t1=$t0=x
                    add_quad(_MUL, "$t1", to_string(4), "$t1");
                }
            }
            //tell addr or value
            if (i < entry->dim) {//addr
                ret += "'$t1";
            } else {//value
                ret += "#$t1";
            }
        }
    } else if (i < entry->dim) {
        add_quad(DIRECT, "move $t1, $0");
        ret += "'$t1";
    }
    if (cons) ret = to_string(to_int(ret));
    print("LVal");
    return ret;
    //can't use const array in funcRparam
//    if (g_entry.iType==I_CONST and g_entry.dim > 0 and g_entry.dim - dim > 0) return 3;
//    return g_entry.dim - dim;
}

vector<int> FuncRParams() {
    vector<int> param;
//    param.push_back(Exp());
    //add_quad(PUSH, Exp());
    int i = 0;
    Exp();
//    add_quad(ASSI, "$s"+to_string(i), "$t1");
    add_quad(PUSH_STACK);
    peek_sym();
    i++;
    while (classes[0] == COMMA) {
        next_sym();
//        param.push_back(Exp());
        Exp();
//        add_quad(ASSI, "$s"+to_string(i), "$t1");
        i++;
        add_quad(PUSH_STACK);
        peek_sym();
    }
    int num=i-1;
    for(int j =0;j<i;j++) {
//        add_quad(POP_STACK);
        add_quad(DIRECT, "lw $t0, "+ to_string(num--*4) + "($sp)");
        add_quad(DIRECT, "sw $t0, "+ to_string(4*i-(base_offset-j*4)) + "($sp)");
//        add_quad(PUSH, "$s"+to_string(j));
    }
    add_quad(DIRECT, "addi $sp,$sp,"+ to_string(4*i));
    print("FuncRParams");
    return param;
}

string UnaryExp() {
    bool op = false;
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
        op = true;
        push = false;
        if (cla == NOT) {
            UnaryExp();
            add_quad(_EQL, "$t1", "0", "$t1");
            ret = "$t1";
        } else if (cla == MINU){
            if (cons) {
                ret = "-";
                ret += UnaryExp();
                if (ret[0] == '-' and ret[1] == '-') {
                    string temp;
                    for (int i = 2; i < ret.size(); i++) temp += ret[i];
                    ret = temp;
                }
            } else {
                ret = new_temp_var();
                string exp = UnaryExp();
//                add_quad(DIRECT, "li $t1, 0");//avoid $t1 crush
                add_quad(_SUB, "0", "$t1", ret);
            }
        } else ret = UnaryExp();//PLUS
    } else ret = PrimaryExp();
    if (!cons and ret != "$t1" and !op) add_quad(ASSI, "$t1", ret);
    print("UnaryExp");
    return ret;
}

string PrimaryExp() {
    peek_sym();
//    int dim=0;
    string ret;
    if (classes[0] == IDENFR) ret = LVal();
    else if (classes[0] == LPARENT) {
        next_sym();
        ret = Exp();
        next_not_error(RPARENT);
    } else if (classes[0] == INTCON) ret = Number();
    else error();
    print("PrimaryExp");
    return ret;
}

string exp_loop(string (*func)(), void (*print_func)(), Symbol sym1, Symbol sym2, Symbol sym3, Symbol sym4) {
    vector<string> vars;
    string ret, temp;
//    Symbol op=DEFAULT;
    int con = uninit;
    vector<Symbol> ops;
    temp = func();
    //add_quad(PUSH_STACK);
    if (open_cal and cons )
        //and (is_digit(temp)!=uninit or search_tab(temp)->iType==I_CONST or global)
        con = to_int(temp);
    else ret = temp;
    peek_sym();
    int n = logic_counter++;
    while (classes[0] == sym1 || classes[0] == sym2 || classes[0] == sym3 || classes[0] == sym4) {
        print_func();
        Symbol symbol = classes[0];//operator
        next_sym();
        if (sym1 == AND or sym1 == OR) {
            //$t1 is target
            if (sym1 == AND) {
                add_quad(DIRECT, "beqz $t1, logic_"+ to_string(n));
            } else add_quad(DIRECT, "bnez $t1, logic_"+ to_string(n));
        }
        if (!cons) add_quad(PUSH_STACK);
        temp = func();
        if (open_cal and cons ) {
            if (con == uninit) {//no con before
                con = to_int(temp);
//                op = symbol;
            }//has con before
            else con = cal(symbol, to_string(con), temp);//const cal here
        } else {
            add_quad(POP_STACK);
            add_quad(sym2op(symbol), "$t0", "$t1", "$t1");
            //add_quad(PUSH_STACK);
        }
        peek_sym();
    }
    if (sym1 == AND or sym1 == OR) add_quad(LABEL, "logic_"+ to_string(n));
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
string Exp_Arr() {
//    int dim = AddExp();
    string name = AddExp();
    if (is_arr) {
        add_quad(SAVEARR, arr_name, to_string((arr_y+arr_x*arr_j)*4), "$t1");
        arr_y++;
        if (arr_i != 1 and arr_y > arr_j-1) {
            arr_y = 0;
            arr_x++;
        }
    }
    print("Exp");
    return name;
//    return dim;
}
string ConstExp() {
    cons = true;
    string ret = AddExp();
    print("ConstExp");
    cons = false;
    return ret;
}
string ConstExp_Arr() {
    cons = true;
//const数组初始化时用cons会错误 暂且放弃一步计算
    string ret = AddExp();
    if (is_arr) {
        if (split(ret, "#").size() > 1) {//arr = arr
            add_quad(ASSI, "$t1", ret);
            ret = "$t1";
        }
        SymEntry entry(I_CONST, 0, to_int(ret));
        insert_tab(arr_name+"#"+to_string((arr_y+arr_x*arr_j)*4), entry);
        add_quad(SAVEARR, arr_name, to_string((arr_y+arr_x*arr_j)*4), ret);
        arr_y++;
        if (arr_i != 1 and arr_y > arr_j-1) {//if 2 dim arr
            arr_y = 0;
            arr_x++;
        }
    }
    print("ConstExp");
    cons = false;
    return ret;
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



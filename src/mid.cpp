#include "mid.h"
#include <cstring>
vector<Quadruple> quads;

ofstream mid("mid.txt");
int t_reg_mid=0, str_no=1;//str0="\n"
int uninit = -1365681;
Op sym2op(Symbol sym) {
    switch (sym) {
        case PLUS: return _ADD;
        case MINU: return _SUB;
        case MULT: return _MUL;
        case DIV: return _DIV;
        case MOD: return _MOD;
        default: return DEF;
    }
}

void add_quad(Op op, string x, string y, string z) {
    quads.push_back(Quadruple(op, x, y, z));
}

string mid_out(Quadruple quad) {
    string ret;
    switch (quad.op) {
        case _ADD:   ret =  quad.z + " = " + quad.x + " + " + quad.y; break;
        case _SUB:   ret =  quad.z + " = " + quad.x + " - " + quad.y; break;
        case _DIV:   ret =  quad.z + " = " + quad.x + " / " + quad.y; break;
        case _MUL:   ret =  quad.z + " = " + quad.x + " * " + quad.y; break;
        case _MOD:   ret =  quad.z + " = " + quad.x + " % " + quad.y; break;
        case ASSI:  ret =  quad.x + " = " + quad.y; break;
        case PARA:  ret =  "para " + quad.x; break;
        case FUNC:  ret =  "\n" + quad.x + " " + quad.y + "()"; break;
        case PUSH:  ret =  "push " + quad.x; break;
        case VAR:
            ret =  "var int " + quad.x;
            if (quad.y != "NULL") ret += " = " + quad.y;
            break;
        case CON:
            ret =  "const int " + quad.x;
            if (quad.y != "NULL") ret += " = " + quad.y;
            break;
        case COMP:  ret =  "cmp " + quad.x + " " + quad.y; break;
        case RET:   ret =  "ret " + quad.x; break;
        case GETRET:ret =  quad.x + " = RET"; break;
        case OUT:   ret =  "printf " + quad.x+"\n"; break;
        case STR:   ret =  "const str " + quad.x + " = \"" + quad.y + "\""; break;
        case CALL:  ret =  "CALL " + quad.x; break;
        case J:     ret =  "J " + quad.x; break;
        case IN:    ret =  quad.x + " = getint()"; break;
        case BZ:    ret =  "bz " + quad.x + " " + quad.y+ " " + quad.z; break;
        case BNZ:   ret =  "bnz " + quad.x + " " + quad.y+ " " + quad.z; break;
        case LABEL: ret =  "label " + quad.x; break;
        case TAB: ret =  "tab";break;
        case DEF:ret =  "DEF";break;
        case PUSH_STACK: ret = "push stack";break;
        case POP_STACK: ret = "pop stack";break;
        case PUSH_GP: ret = "push gp";break;
        case POP_GP: ret = "pop gp";break;
        default:    cout << "UNKNOWN";
    }
    return ret + "\n";
}

void print_quad() {
    for (Quadruple quad : quads) mid << mid_out(quad);
}

string new_temp_var() {
//    if (++t_reg_mid >= 8) t_reg_mid=0;//t reg loop
////    insert_tab("@s" + to_string(t_reg_mid++), SymEntry(I_TEMP));
//    return "$s" + to_string(t_reg_mid);
    return "$t1";
}

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if("" == str) return res;
    char * strs = new char[str.length() + 1] ;
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        res.emplace_back(p);
        p = strtok(NULL, d);
    }
    return res;
}

string new_str(string value) {
    vector<Quadruple>::iterator ite = quads.begin();
    quads.insert(ite, 1, Quadruple(STR, "str"+to_string(str_no), value, ""));
    insert_tab("str"+to_string(str_no), SymEntry(I_STR, 1, 1, 1));
    return "str"+to_string(str_no++);
}

int is_digit(string str) {
    if (str == "0") return 0;
    int value = atoi(str.c_str());
    if (value != 0) return value;
    else return uninit;
}

int to_int(string str) {//return int. if not int, return uninit
    //digit
    if(is_digit(str)!=uninit) return is_digit(str);
    //const
    else return search_tab(str)->value;
}

int cal(Symbol op, string x, string y) {
    int a = to_int(x), b = to_int(y);
    if (a == uninit or b == uninit) return uninit;
    switch (op) {
        case PLUS: return a+b;
        case MINU: return a-b;
        case MULT: return a*b;
        case DIV: return a/b;
        case MOD: return a%b;
    }
    return uninit;
}
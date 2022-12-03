#include "mips.h"
#include "sym.h"
#include "mid.h"
#include "err.h"
ofstream mips("mips.txt");
bool global_declaring = true;
int gp_offset = 0;
//int gp_stack = 1000;
int fp_offset = 0;
int t_reg=0;
int pre_tab=0;
bool stack_prepare = true, done = false;
int var_offset = 0, para_reg_no = 1, para_offset=0, push_reg_no = 1;
int space_for_var = 4000;
int max_para_reg = 0;
bool mips_on = false;
vector<string> ids;
vector<int> addrs;
bool para_fix = false;//fix fp shift in func&para stage, func can't search so para does it
vector<string> preserve_regs = {
        "$fp", "$ra",
//                                "$s0",
//    "$s1", "$s2", "$s3",
//                                "$s4", "$s5", "$s6", "$s7"
};
int base_offset = space_for_var + 4 * preserve_regs.size();


void addi(const string& x, int num=4, string y="") {
    if (y.empty()) y = x;//y = x in default
    mips << "addi "+x+", "+y+", "+to_string(num) << endl;
}

void lw(const string& x, int off=4, const string& y="$fp") {
    mips << "lw "+x+", "+ to_string(off)+"("+y+")" << endl;
}

void sw(const string& x, int off=4, const string& y="$sp") {
    mips << "sw "+x+", "+ to_string(off)+"("+y+")";
}

void load_digit(string dig) {
    mips << "li "+ t()+", "+dig+"\n";
}

void save(const string& x, string value="", bool decl = false) {//save a var to mem
    SymEntry *var;
    if (decl) var = search_pre_tab(x);
    else var = search_tab(x);
    if(value == "" or value == "NULL") value = to_string(to_int(x));//value=value in tab in default
    if (value == "RET") value = "$v0";
    if (value[0] == '$') {
        if (x[0] == '$') {//$ to $
            mips << "move "+ x+", "+ value;
            return;
        }
        if (var->addr == uninit) {//no addr before
            if (var->global) {
                sw(value, gp_offset, "$gp");
                var->addr = gp_offset;
                gp_offset += 4;
            } else {//local var
                sw(value, fp_offset, "$fp");
                var->addr = fp_offset;
                fp_offset+=4;
            }
        } else //has addr
            sw(value, var->addr, var->global?"$gp":"$fp");
//        sw(value, var->addr, var->global ? "$gp" : "$fp");
        return;
    }
    if (value != to_string(uninit)) {//save digit
        load_digit(value);
        value = t(-1);
    }
    if (value == to_string(uninit)) mips << "# ";
    if (var->addr == uninit) {//no addr before
        if (var->global) {
            sw(value, gp_offset, "$gp");
            var->addr = gp_offset;
            gp_offset += 4;
        } else {//local var
            sw(value, fp_offset, "$fp");
            var->addr = fp_offset;
            fp_offset+=4;
        }
    } else //has addr
        sw(value, var->addr, var->global?"$gp":"$fp");
}

string t(int last) {
    if (last == -1) return "$t" + to_string(t_reg);
    if (++t_reg >= 9) t_reg=2;//t reg loop
    //t0 and t1 are preserved
    return "$t" + to_string(t_reg);
}

string get(string var, string to="$t0") {//get var from mem to a reg
    //arr value
    vector<string> strs = split(var, "#");
    if (strs.size() > 1) {
        //$t1=offset
        string arr = strs[0];
        SymEntry *entry = search_tab(arr, true);
        print_tab();
        if (to == "$t0") to = t();
        if (entry->iType == I_PARA and entry->dim > 0)//para refer
            mips << "add $t1, $t1, "+ get(arr)<< endl;
        else//solid arr
            mips << "addi $t1, $t1, "+ to_string(entry->addr)<< endl;
        mips << "add $t1, $t1, "<<((entry->global)?"$gp":"$fp")<< endl;
        lw(to, 0, "$t1");
        return to;
    }
    //arr addr
    strs = split(var, "'");
    if (strs.size() > 1) {
        //$t1=offset
        string arr = strs[0];
        SymEntry *entry = search_tab(arr, true);
        print_tab();
        if (to == "$t0") to = t();
        if (entry->iType != I_PARA) {//first time
            mips << "addi $t1, $t1, " + to_string(entry->addr) << endl;
        }
        else
            mips << "add $t1, $t1, "+ get(arr)<< endl;
        if (entry->global) {//all turned to fp-based
            mips << "add $t1, $t1, "<<"$fp"<< endl;
            mips << "sub $t1, $t1, "<<"$gp"<< endl;
        }
//        mips << "addi $t1, $t1, "+ to_string(base_offset)<< endl;//fp's coming shift
//        lw(to, 0, "$t1");
        return "$t1";//offset based on present fp
    }
    //RET
    if (var == "RET") return "$v0";
    //digit
    if (is_digit(var) != uninit) {
        load_digit(var);
        return t(-1);
    }
    //reg
    if (var[0] == '$') return var;//if reg, return reg
    //in stack
    SymEntry *entry = search_tab(var);//add addr filter here to avoid var covered
//    if (entry->reg != "") return entry->reg;
//    //const
//    if (entry->iType==I_CONST) {
//        load_digit(to_string(entry->value));
//        return t(-1);
//    }
    //var
//    int value = to_int(var);
//    if (value == uninit) cout << "failed to get data from "+var<<endl;
    if (to == "$t0") to = t();
    lw(to, entry->addr, entry->global?"$gp":"$fp");
    return to;
}

string get_addr(const string& var) {
    SymEntry *entry = search_tab(var);
    return to_string(entry->addr)+"("+(entry->global?"$gp":"$fp")+")";
}

string get_value(const string& var) {
    return "";
}

void push(string reg) {
    mips << "sub $sp,$sp,4\nsw "+reg+", ($sp)";
}

void pop(string reg) {
    mips << "lw "+reg+", ($sp)\n"
                      "addi $sp,$sp,4";
}

void meta(string to, string from) {
    to = get(to);
    from = get(from);

}

string remark(Quadruple quad) {
    return "   # "+ mid_out(quad);
}

void insert_para(string id) {
    ids.push_back(id);
    addrs.push_back(fp_offset);
    for (int i = 0; i < ids.size(); i++)
        search_pre_tab(ids[i])->addr=addrs[addrs.size()-i-1];
    fp_offset+=4;
}

void print_mips() {
    mips_on = true;
//    mips << ".data" << endl;
    for (int i = 0; i < quads.size(); i++) {
        Quadruple quad = quads[i];
//        cout << to_string(quad.op) << endl;
        Quadruple next = i+1 < quads.size() ? quads[i+1] : Quadruple(DEF,"","","");
        switch (quad.op) {
            case _ADD:   mips << "add " + get(quad.z)+", " + get(quad.x)+", " + get(quad.y); break;
            case _SUB:   mips << "sub " + get(quad.z)+", " + get(quad.x)+", " + get(quad.y); break;
            case _DIV:
                mips << "div " + get(quad.x)+", " + get(quad.y)+"\n";
                mips << "mflo " + get(quad.z);
                break;
            case _MUL:
                mips << "mult " + get(quad.x)+", " + get(quad.y)+"\n";
                mips << "mflo " + get(quad.z);//TODO if mfhi is not empty
                break;
            case _MOD:
                mips << "div " + get(quad.x)+", " + get(quad.y)+"\n";
                mips << "mfhi " + get(quad.z);
                break;
            case _AND:   mips << "and " + get(quad.z)+", " + get(quad.x)+", " + get(quad.y); break;
            case _OR:    mips << "or " + get(quad.z)+", " + get(quad.x)+", " + get(quad.y); break;
            case _EQL: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal eql"; break;
            case _NEQ: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal neq"; break;
            case _LSS: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal lss"; break;
            case _LEQ: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal leq"; break;
            case _GRE: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal gre"; break;
            case _GEQ: mips << "move $t0, " + get(quad.x) +
                               "\nmove $t1, " + get(quad.y) +
                               "\njal geq"; break;
            case ASSI:   save(quad.x, get(quad.y)); break;
            case PARA: {
//                lw("$s" + to_string(reg_no++), var_offset++ * 4, "$fp");
                if (para_reg_no > max_para_reg) {
//                    insert_para(quad.x);
                    search_pre_tab(quad.x)->addr = fp_offset;
                    fp_offset+=4;
                } else {
                    mips << "move $s" + to_string(para_reg_no) + ", $a" + to_string(para_reg_no);
                    search_pre_tab(quad.x)->reg = "$s" + to_string(para_reg_no++);
                }
                if (search_pre_tab(quad.x)->dim>0) {//refer
                    mips << "add $t0, "+get(quad.x)+", $v1\n";
                    save(quad.x, "$t0");
                }
                break;
            }
            case FUNC:
                if (!done) {
                    mips << endl << "j func_main\n"
                                    "nop\n"
                                    "true:\n"
                                    "li $t1, 1\n"
                                    "jr $ra\n"
                                    "false:\n"
                                    "li $t1, 0\n"
                                    "jr $ra\n"
                                    "gre:\n"
                                    "bgt $t0, $t1, true\n"
                                    "j false\n"
                                    "geq:\n"
                                    "bge $t0, $t1, true\n"
                                    "j false\n"
                                    "lss:\n"
                                    "blt $t0, $t1, true\n"
                                    "j false\n"
                                    "leq:\n"
                                    "ble $t0, $t1, true\n"
                                    "j false\n"
                                    "eql:\n"
                                    "beq $t0, $t1, true\n"
                                    "j false\n"
                                    "neq:\n"
                                    "bne $t0, $t1, true\n"
                                    "j false";//init
                    done = true;
                }
                mips << endl <<  "#----------------------\n"
                                 "func_"+quad.y+":\n";
                global_declaring = false;
                if (quad.y == "main") stack_prepare = false;
                if (stack_prepare) {
//                    mips << "\nmove $sp, $fp\n";
                    for (int i=0; i < preserve_regs.size(); i++) {
                        push(preserve_regs[i]);
                        mips << endl;
                    }
                    para_offset = search_tab(quad.y, true)->param.size()*4;
                    mips << "sub $sp, $sp, " + to_string(space_for_var)+ "\n"//TODO $fp = $sp - space_for_variables
                                                                         "sub $v1, $fp, $sp\n"
                                                                         "move $fp, $sp    # prolog\n";
                }
                //is flow when compiling, so just need to increase
                fp_offset=0;ids.clear();addrs.clear();//clear former data for paras next
//                para_fix = true;
                continue;
            case PUSH: {
//                mips << endl;
                if (push_reg_no > max_para_reg) //overflow
                    mips << "sw "+get(quad.x)+", "+ to_string(-base_offset)+"($sp)";
                else mips << "move $a"+to_string(push_reg_no++)+", "+ get(quad.x);
                break;
            }
            case VAR: save(quad.x, quad.y, true);break;
            case CON: save(quad.x, quad.y, true);break;
            case COMP:  mips << "cmp " << quad.x << " " << quad.y; break;
            case RET:
                if (stack_prepare) {
                    if (quad.x != "NULL" and quad.x != "")mips << "move $v0, " + get(quad.x) + "\n";
                    mips << "addi $sp, $fp, "+to_string(space_for_var)+"\n";
                    for (int i=0; i < preserve_regs.size(); i++) {
                        pop(preserve_regs[preserve_regs.size() - i - 1]);
                        mips << endl;
                    }
                    mips << "jr $ra    # epilog\n";
                }
                var_offset = 0; para_reg_no = 1;
                break;
            case IN:mips << "li $v0, 5\n"
                            "syscall\n";
                save(quad.x, "$v0"); break;
            case GETRET:
                save(quad.x, "$v0"); break;
            case OUT: {
                if (search_tab(quad.x)->iType == I_STR) {//str
                    mips << "\nli $v0, 4\n"
                            "la $a0, " + quad.x + "\n"
                                                  "syscall";
                } else {//reg or var
                    mips << "\nli $v0, 1\n"
                            "move $a0, "+get(quad.x) + "\n"
                                                       "syscall";
                }
                break;
            }
            case STR:
                mips << quad.x+": .asciiz \"" <<  quad.y + "\"\n";
                if (global_declaring and next.op != STR) mips << "\n.text\n"
                                                                 "move $fp, $sp\n\n";//init
                continue;
            case CALL:
                mips << "jal func_" << quad.x + "\n";
                var_offset = 0; push_reg_no = 1;
                break;
            case J:     mips << "J " << quad.x; break;
            case BZ:    mips << "bz " << quad.x << " " << quad.y<< " " << quad.z; break;
            case BNZ:   mips << "bnz " << quad.x << " " << quad.y<< " " << quad.z; break;
            case LABEL: mips << quad.x+":"; break;
            case TAB:
                preTab = tab_flow[pre_tab++];
//                print_tab();
                break;
            case PUSH_STACK:
                push("$t1");break;
            case POP_STACK:
                pop("$t0");break;
//            case PUSH_GP: gp_stack-=4;mips << "sw $t1, "+to_string(gp_stack)+"($gp)";break;
//            case POP_GP: mips << "lw $t0, "+to_string(gp_stack)+"($gp)";gp_stack+=4;break;
            case DEF: break;
            case DIRECT: mips << quad.x;break;
            case ARR: {
                //arr name x y
                SymEntry *arr = search_tab(quad.x, true);
                if (arr->global) {
                    arr->addr = gp_offset;
                    gp_offset += 4 * arr->i * arr->j;
                } else {//local arr
                    arr->addr = fp_offset;
                    fp_offset += 4 * arr->i * arr->j;
                }
                break;
            }
            case GETARR: {
                //getarr name offset des
                SymEntry *arr = search_tab(quad.x, true);
                break;
            }
            case SAVEARR: {
                //savearr name offset value="$t1"
                SymEntry *arr = search_tab(quad.x, true);
                if (quad.z != "$t1")
                    mips << "move $t1, "+get(quad.z) + "\n";
                if (quad.y != "$t0") sw("$t1", arr->addr + to_int(quad.y), arr->global?"$gp":"$fp");
                else {//from lval = exp
                    mips << "add $t0, $t0, " << (arr->global?"$gp":"$fp") << "\n"
                                                                             "add $t0, $t0, " + to_string(arr->addr)+
                                                                             "\nsw $t1, ($t0)\n";
                }
                if (arr->global) gp_offset+=4;
                else fp_offset+=4;
                break;
            }
            default:    cout << "UNKNOWN\n" << endl;
        }
        mips << remark(quad);//has endl
    }
}


#include "mips.h"
ofstream mips("mips.txt");
bool global_declaring = true;
int gp_offset = 0, sp_offset=0;
int gp_stack = 1000;
//vector<int> fp_offset={0};
int fp_offset = 0;
//int fp_now=0;
int t_reg=0;
int pre_tab=0;
bool stack_prepare = true, not_first = false, done = false;
int var_offset = 0, para_reg_no = 1, para_offset=0, push_reg_no = 1;
int space_for_var = 4000;
int max_para_reg = 0;
bool mips_on = false;
vector<string> ids;
vector<int> addrs;
vector<string> preserve_regs = {
        "$sp", "$ra", "$fp",
//                                "$s0",
//    "$s1", "$s2", "$s3",
//                                "$s4", "$s5", "$s6", "$s7"
};
int base_offset = space_for_var + 4*preserve_regs.size()*2;

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
    if (value != "uninit") {//save digit
        load_digit(value);
        value = t(-1);
    }
    if (value == "") mips << "# ";
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
        if (++t_reg >= 9) t_reg=0;//t reg loop
        return "$t" + to_string(t_reg);
}

string get(string var, string to="$t0") {//get var from mem to a reg
    //RET
    if (var == "RET") return "$v0";
    //digit
    if (is_digit(var) != uninit) {
        load_digit(var);
        return t(-1);
    }
    //reg
    if (var[0] == '$') return var;//if reg, return reg
    //saved in a reg
    SymEntry *entry = search_tab(var);
    if (entry -> addr == uninit) {
        break_point();
    }
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

string space() {
    return to_string(space_for_var + 4*preserve_regs.size());
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
            case ASSI:   save(quad.x, get(quad.y)); break;
            case PARA: {
                mips << endl;
//                lw("$s" + to_string(reg_no++), var_offset++ * 4, "$fp");
                if (para_reg_no > max_para_reg) {
                    insert_para(quad.x);
//                    search_pre_tab(quad.x)->addr = fp_offset;
//                    fp_offset+=4;
                } else {
                    mips << "move $s" + to_string(para_reg_no) + ", $a" + to_string(para_reg_no);
                    search_pre_tab(quad.x)->reg = "$s" + to_string(para_reg_no++);
                }
                break;
            }
            case FUNC:
                if (!done) {
                    mips << endl << "j func_main\n"
                                    "nop";//init
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
                    //TODO push S reg that may alter
                    mips << "sub $fp, $sp, " + space() + "\n"//TODO $fp = $sp - space_for_variables
                            "move $sp, $fp    # prolog\n";
                }
                //is flow when compiling, so just need to increase
                fp_offset=0;ids.clear();addrs.clear();
                continue;
            case PUSH: {
                mips << endl;
                if (push_reg_no > max_para_reg) {//overflow
                    mips << "sw "+get(quad.x)+", "+ to_string(para_offset-base_offset)+"($fp)";
                    para_offset+=4;
                } else mips << "move $a"+to_string(push_reg_no++)+", "+ get(quad.x);
                break;
            }
            case VAR: save(quad.x, quad.y, true);break;
            case CON: save(quad.x, quad.y, true);break;
            case COMP:  mips << "cmp " << quad.x << " " << quad.y; break;
            case RET:
                if (stack_prepare) {
                    if (quad.x != "NULL" and quad.x != "")mips << "move $v0, " + get(quad.x) + "\n\n";
                    mips << "\naddi $fp, $fp, "+space()+"    # epilog\n"//TODO $fp = $sp + space_for_variables
                            "move $sp, $fp\n";
                    for (int i=0; i < preserve_regs.size(); i++) {
                        pop(preserve_regs[preserve_regs.size() - i - 1]);
                        mips << endl;
                    }
                    mips << "jr $ra";
                }
//                fp_now--;
//                fp_offset.pop_back();
                var_offset = 0; para_reg_no = 1; para_offset = 0;
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
                                                                 "li $fp, 0x10040000\n"
                                                    "move $sp, $fp\n\n";//init
                continue;
            case CALL:
                mips << "\njal func_" << quad.x;
                var_offset = 0; push_reg_no = 1;para_offset = 0;
                break;
            case J:     mips << "J " << quad.x; break;
            case BZ:    mips << "bz " << quad.x << " " << quad.y<< " " << quad.z; break;
            case BNZ:   mips << "bnz " << quad.x << " " << quad.y<< " " << quad.z; break;
            case LABEL: mips << "label " << quad.x; break;
            case TAB:
                preTab = tab_flow[pre_tab++];
//                print_tab();
                break;
            case PUSH_STACK:
                push("$t1");break;
            case POP_STACK:
                pop("$t0");break;
            case PUSH_GP: gp_stack-=4;mips << "sw $t1, "+to_string(gp_stack)+"($gp)";break;
            case POP_GP: mips << "lw $t0, "+to_string(gp_stack)+"($gp)";gp_stack+=4;break;
            case DEF: break;
            default:    cout << "UNKNOWN\n" << endl;
        }
        mips << remark(quad);//has endl
    }
}


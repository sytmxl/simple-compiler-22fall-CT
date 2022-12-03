#include "err.h"
#include "mips.h"
#include <map>
#include "mid.h"
//TODO more info showed when error occurs
//"int ok" trigger print_tab
bool debug = true;
//bool more_info= debug;// if show more info in error.txt?
bool line_protection = false;//if one line has one problem only?
Tab *preTab=nullptr;
int last_line=0;
map<int, char> errors;
vector<Tab*> tab_flow;

void break_point();

void tab_init() {
    new_tab();
}

void print_error() {
    for (auto error : errors) err << error.first << " " << error.second << endl;
}

void new_tab(bool local) {
    Tab *newTab = new Tab (preTab, local);
    preTab = newTab;
    tab_flow.push_back(preTab);
    add_quad(TAB);
}

void pop_tab() {
//    print_quad();
//    print_mips();
    preTab = preTab->parent;
    tab_flow.push_back(preTab);
    add_quad(TAB);
}

SymEntry* search_tab(string id, bool off) {
    Tab *pre = preTab;
    while (pre->parent != nullptr) {
        if (pre->tab.count(id)) {
            if (mips_on and !off) {
                if (pre->tab[id].addr != uninit) return &pre->tab[id];
            } else return &pre->tab[id];
        }
        pre = pre->parent;
    }
    //at last tab(root tab or local tab)
    if (pre->tab.count(id)) {
        if (mips_on and !off) {
            if (pre->tab[id].addr != uninit) return &pre->tab[id];
        } else return &pre->tab[id];
    }
//    SymEntry null(I_NULL);
    return new SymEntry(I_NULL);
}

SymEntry* search_pre_tab(string id) {
    if (preTab->tab.count(id)) return &(preTab->tab[id]);
    return new SymEntry(I_NULL);
}

void insert_tab(const string &id, const SymEntry &entry, Tab *tab) {
    if (search_pre_tab(id)->iType==I_NULL) tab->tab[id] = entry;
//    else error('b');
}

void print_tab() {
    Tab *pre = preTab;
    while (pre->parent != nullptr) {
        cout << "-------" << endl;
        for (const auto& entry : pre->tab)
            cout << entry.first << " ";
        cout << endl;
        pre = pre->parent;
    }
    cout << "-------" << endl;
    for (const auto& entry : pre->tab)
        cout << entry.first << " ";
    cout << endl;
    cout << "~~~~~~~~~~~~~~~~~"<< endl;
}

void error(char errn, int line_number) {
    if (line_protection and last_line == line_number) return;

//    string output=to_string(line_number + 1) + " " + errn;
//    if (more_info) output = output + " : " + sym + " @ " + line;
//    if (setting) err_buffer.push_back(output);
//    else if (peeking) {}
//    else err << output << endl;
    if (setting) err_buffer[line_number] = errn;
    else if (peeking) {}
    else {
        errors[line_number] = errn;
        last_line = line_number;
    }
    //for debug
    if (debug) cout << line_number << " " << errn << endl;
    if (errn == 'X') break_point();
}

void break_point() {
    err << line << endl;
//    next_sym();
}

void sym2error(Symbol sym) {//error map for phasing
    if (sym == SEMICN) error('i');
    else if (sym == RPARENT) error('j');
    else if (sym == RBRACK) error('k');
}
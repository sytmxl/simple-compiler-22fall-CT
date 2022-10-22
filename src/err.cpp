#include "err.h"
#include <map>
bool debug = true;
bool more_info= debug;// if show more info in error.txt?
bool line_protection = !debug;//if one line has one problem only?
Tab *preTab=NULL;
int last_line=0;
map<int, char> errors;

void tab_init() {
    new_tab();
}

void print_error() {
    for (auto error : errors) err << error.first << " " << error.second << endl;
}

void new_tab(bool local) {
    Tab *newTab = new Tab{.parent=preTab, .local=local};
    preTab = newTab;
}

void pop_tab() {
    preTab = preTab->parent;
}

SymEntry search_tab(string id) {
    Tab *pre = preTab;
    while (pre->parent != NULL && !pre->local) {
        if (pre->map.count(id)) return pre->map[id];
        pre = pre->parent;
    }
    //at last tab(root tab or local tab)
    if (pre->map.count(id)) return pre->map[id];
    SymEntry null = {I_NULL};
    return null;
}

void insert_tab(const string &id, const SymEntry &entry) {
    if (search_tab(id).iType==I_NULL) preTab->map[id] = entry;
//    else error('b');
}

void print_tab() {
    Tab *pre = preTab;
    while (pre->parent != NULL) {
        cout << "-------" << endl;
        for (const auto& entry : pre->map)
            cout << entry.first << " ";
        cout << endl;
        pre = pre->parent;
    }
    cout << "-------" << endl;
    for (const auto& entry : pre->map)
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
    else errors[line_number] = errn;
    last_line = line_number;
}

void sym2error(Symbol sym) {//error map for phasing
    if (sym == SEMICN) error('i');
    else if (sym == RPARENT) error('j');
    else if (sym == RBRACK) error('k');
}
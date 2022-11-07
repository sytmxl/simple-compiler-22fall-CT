#ifndef CT_SYM_H
#define CT_SYM_H
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>

using namespace std;
enum Symbol {
    DEFAULT=0,IDENFR,INTCON,STRCON,MAINTK,CONSTTK,INTTK,
    BREAKTK,CONTINUETK,IFTK,ELSETK,
    WHILETK,GETINTTK,PRINTFTK,RETURNTK,
    NOT, AND, OR, PLUS, MINU,VOIDTK,
    MULT, DIV, MOD,LSS, LEQ, GRE, GEQ, EQL, NEQ,
    ASSIGN, SEMICN, COMMA,LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

extern Symbol cla; //pre sym
extern vector<string> buffer;
extern map<int, char> err_buffer;
extern vector<Symbol> classes;
extern bool is_end, setting, peeking;
extern ofstream out, err;
extern int line_no, inside_str;
extern int inside_str;
extern string line, sym;
extern map<string, Symbol> to_cla;
extern string get_sym[];

void next_sym();
void peek_sym(int num=1);
void set(bool flush_only=false);
void revert();

#endif // CT_SYM_H
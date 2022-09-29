#ifndef CT_SYM_H
#define CT_SYM_H
#include <string>
#include <iostream>
#include "fstream"
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "map"
#include "vector"

using namespace std;

enum Symbol {
    DEFAULT=0,
    IDENFR,
    INTCON,STRCON,
    MAINTK,
    CONSTTK,INTTK,
    BREAKTK,CONTINUETK,
    IFTK,ELSETK,
    WHILETK,GETINTTK,PRINTFTK,RETURNTK,
    NOT, AND, OR, PLUS, MINU,
    VOIDTK,
    MULT, DIV, MOD,
    LSS, LEQ, GRE, GEQ, EQL, NEQ,
    ASSIGN, SEMICN, COMMA,
    LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

void next_ch();
void next_sym();
void read_dig();
void read_str();
void read_id();
void read_other();
void save(string str, Symbol symbol=DEFAULT);
void read_note();
void init_sym();
void peek_sym(int num=1);
void set(bool flush_only=false);
void revert();
void end();
#endif // CT_SYM_H
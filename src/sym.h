#ifndef SYMBOL_H_
#define SYMBOL_H_
#include <string>
#include <iostream>
#include "fstream"
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "map"

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
string get_sym[] = {
    "DEFAULT",
    "IDENFR",
    "INTCON","STRCON",
    "MAINTK",
    "CONSTTK","INTTK",
    "BREAKTK","CONTINUETK",
    "IFTK","ELSETK",
    "WHILETK","GETINTTK","PRINTFTK","RETURNTK",
    "NOT", "AND", "OR", "PLUS", "MINU",
    "VOIDTK",
    "MULT", "DIV", "MOD",
    "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ",
    "ASSIGN", "SEMICN", "COMMA",
    "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE",
};

map<string, Symbol> to_sym{
    {"main", MAINTK},
    {"const", CONSTTK},
    {"int", INTTK},
    {"break", BREAKTK},
    {"continue", CONTINUETK},
    {"if", IFTK},
    {"else", ELSETK},
    {"!", NOT},{"&&", AND},{"||", OR},
    {"while", WHILETK},
    {"getint", GETINTTK},
    {"printf", PRINTFTK},
    {"return", RETURNTK},
    {"+", PLUS},
    {"-", MINU},
    {"void", VOIDTK},
    {"*", MULT},
    {"/", DIV},
    {"%", MOD},
    {"<", LSS},
    {"<=", LEQ},
    {">", GRE},
    {">=", GEQ},
    {"==", EQL},
    {"!=", NEQ},
    {"=", ASSIGN},
    {";", SEMICN},
    {",", COMMA},
    {"(", LPARENT},
    {")", RPARENT},
    {"[", LBRACK},
    {"]", RBRACK},
    {"{", LBRACE},
    {"}", RBRACE},
};

void next_ch();
void read_sym();
void read_dig();
void read_str();
void read_id();
void read_other();
void save(string str, Symbol symbol=DEFAULT);
void read_note();
#endif // SYMBOL_H_
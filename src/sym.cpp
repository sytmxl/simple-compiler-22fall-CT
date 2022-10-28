#include "sym.h"
#include "err.h"
string get_sym[] = {
        "DEFAULT","IDENFR","INTCON","STRCON","MAINTK",
        "CONSTTK","INTTK","BREAKTK","CONTINUETK","IFTK","ELSETK",
        "WHILETK","GETINTTK","PRINTFTK","RETURNTK",
        "NOT", "AND", "OR", "PLUS", "MINU",
        "VOIDTK","MULT", "DIV", "MOD","LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ",
        "ASSIGN", "SEMICN", "COMMA","LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE",
};

map<string, Symbol> to_cla{
        {"main", MAINTK},{"const", CONSTTK},{"int", INTTK},
        {"break", BREAKTK},{"continue", CONTINUETK},{"if", IFTK},{"else", ELSETK},
        {"!", NOT},{"&&", AND},{"||", OR},
        {"while", WHILETK},{"getint", GETINTTK},{"printf", PRINTFTK},{"return", RETURNTK},
        {"+", PLUS},{"-", MINU},
        {"void", VOIDTK},{"*", MULT},{"/", DIV},
        {"%", MOD},{"<", LSS},{"<=", LEQ},{">", GRE},{">=", GEQ},{"==", EQL},{"!=", NEQ},{"=", ASSIGN},{";", SEMICN},{",", COMMA},{"(", LPARENT},{")", RPARENT},{"[", LBRACK},{"]", RBRACK},{"{", LBRACE},{"}", RBRACE},
};
ifstream in("testfile.txt");
ofstream out("output.txt");
Symbol cla;
vector<Symbol> classes;   //when peeking, store symbols' contents and classes
//for error
int line_no = 1;
int word_no = 0; //position of word's first ch
int ch_no = 0; //position of pre ch
ofstream err("error.txt");
int inside_str = 0;

vector<string> lines;
string line;
char ch = ' ';
bool is_end = false;
string sym, t_sym;
Symbol t_cla;
vector<string> syms, buffer;
map<int, char> err_buffer;
bool peeking = false;
bool setting = false;
int t_line_no, t_ch_no, t_word_no;

void next_ch();
void read_dig();
void read_str();
void read_id();
void read_other();
void save(string str, Symbol symbol=DEFAULT);
void read_note();

void next_sym() {
    if (lines.empty()) {
        lines.push_back(" ");
        while (getline(in, line)) lines.push_back(line);
        line = lines[1];
        ch = line[0];
    }
    while ((ch == '\0' ||isspace(ch)) && !is_end) next_ch();
    word_no = ch_no;
    if (isdigit(ch)) read_dig();
    else if (isalpha(ch) || ch == '_') read_id();
    else read_other();

    if (sym == "bp" and debug) break_point();
}

void save(string str, Symbol symbol) {
    if (symbol == DEFAULT) symbol = to_cla[str];
    if (symbol == DEFAULT) return; //delete NUL at the end of file
    if (peeking) {
        syms.push_back(str);
        classes.push_back(symbol);
    } else if (setting) {
        sym = str; cla = symbol;
        buffer.push_back(get_sym[symbol] + ' ' + str);
    } else {
        sym = str; cla = symbol;
        out << get_sym[symbol] + ' ' + str << endl;
//        cout << get_sym[symbol] + ' ' + str << endl;
    }
}
//switch mode, save data, clear vector, read syms, restore data, switch mode
void peek_sym(int num) {
    peeking = true;
    int t_line_no=line_no, t_ch_no=ch_no, t_word_no=word_no;
    syms.clear(); classes.clear();
    for(int i=0; i < num && !is_end; i++) {
        next_sym();
    }
    line_no=t_line_no; ch_no=t_ch_no; word_no=t_word_no;
    line = lines[line_no]; ch = line[ch_no];
    peeking = false;
}

void set(bool flush_only) {//for recall: store things
    setting = !flush_only;
    for (auto &i : buffer) out << i << endl;
    buffer = {};
    errors.insert(err_buffer.begin(),err_buffer.end());
    err_buffer.clear();
    t_line_no=line_no; t_ch_no=ch_no; t_word_no=word_no; t_sym=sym, t_cla=cla;
}

void revert() {//for recall: restore things without outputting
    setting = false;
    buffer = {};
    err_buffer.clear();
    line_no=t_line_no; ch_no=t_ch_no; word_no=t_word_no; sym=t_sym; cla=t_cla;
    line = lines[line_no]; ch = line[ch_no];
}

void next_ch() {
    if (ch_no == line.size()) {
        if (line_no >= lines.size()) {
            is_end = true;
        } else {
            line = lines[++line_no];
//            if (debug) cout << "line: "+line_no << endl;
            ch_no = 0; word_no = 0;
            ch = line[0];
        }
    } else {
        ch = line[++ch_no];
    }
//    if (debug) cout << "ch: "<< ch_no << endl;
}

void read_dig() {
    string str;
    while (isdigit(ch)) {
        str.push_back(ch);
        next_ch();
    }
    save(str, INTCON);
}

void read_str() {
    string str;
    inside_str = 0;
    bool has_error = false;
    while (ch != '\"') {
        if (ch == 32 or ch == 33 or (ch >= 40 and ch <= 126)) {//'\'=92
            if (ch == '\\') {
                str.push_back(ch);
                next_ch();
                if (ch == '\"') { has_error = true;break; }
                if (ch != 'n') has_error = true;//only \n
            }
        } else if (ch == '%') {//'%'=37
            str.push_back(ch);
            next_ch();
            if (ch == '\"') { has_error = true;break; }
            if (ch != 'd') has_error = true;//only %d
            else inside_str++;
        } else has_error = true;
        str.push_back(ch);
        next_ch();
    }//ch == "
    if (has_error) error('a');
    next_ch();  //different form read_dig
    save("\""+str+"\"", STRCON);
}

void read_id() {
    string str;
    while (isalnum(ch) || ch == '_') {
        str.push_back(ch);
        next_ch();
    }
    if (to_cla[str] == 0) save(str, IDENFR);
    else save(str);
}

void read_other() {
    switch(ch) {
        case '\"': next_ch();read_str(); break;
        case '>': next_ch();
            if (ch == '=') {
                next_ch();
                save(">=");
            } else {
                save(">");
            }
            break;
        case '<': next_ch();
            if (ch == '=') {
                next_ch();
                save("<=");
            } else {
                save("<");
            }
            break;
        case '!': next_ch();
            if (ch == '=') {
                next_ch();
                save("!=");
            } else {
                save("!");
            }
            break;
        case '=': next_ch();
            if (ch == '=') {
                next_ch();
                save("==");
            } else save("=");
            break;
        case '/': next_ch();
            if (ch == '/' || ch == '*') read_note();
            else save("/");
            break;
        case '|': next_ch();
            if (ch == '|') {
                next_ch();
                save("||");
            }
            break;
        case '&': next_ch();
            if (ch == '&') {
                next_ch();
                save("&&");
            }
            break;
        default:
            string str;
            str.push_back(ch);
            save(str); //error when not covered
            next_ch();
    }
}

void read_note() {
    if (ch == '/') {
        line = lines[++line_no];
        ch_no = 0;word_no=0;
        ch = line[0];
    } else if (ch == '*') {
        next_ch();
        char pre_ch;
        while (!is_end and !(pre_ch == '*' and ch == '/')) {
            pre_ch = ch;
            next_ch();
        }
        next_ch();
    }
    next_sym();
}



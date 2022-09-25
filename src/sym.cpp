#include "sym.h"

ifstream in("testfile.txt");
ofstream out("output.txt");
int line_no = 0;
int word_no = 0; //position of word's first ch
int ch_no = 0; //position of pre ch
string line, store;
char ch = ' ';
bool is_end = false;

int main() {
    while (!is_end) read_sym();
    out << store;
    in.close();out.close();
}

void read_sym() {
    while ((ch == '\0' ||isspace(ch)) && !is_end) next_ch();
    word_no = ch_no;
    if (isdigit(ch)) read_dig();
    else if (isalpha(ch) || ch == '_') read_id();
    else read_other();
};

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
    while (ch != '\"') {
        str.push_back(ch);
        next_ch();
    }
    next_ch();  //different form read_dig
    save("\""+str+"\"", STRCON);
}

void read_id() {
    string str;
    while (isalnum(ch)) {
        str.push_back(ch);
        next_ch();
    }
    if (to_sym[str] == 0) save(str, IDENFR);
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
            } else {
                save("=");
            }
            break;
        case '/': next_ch();
            if (ch == '/' || ch == '*') read_note();
            else {
                save("/");
            }
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
        getline(in, line);
        line_no++;
        ch_no = 0;
        ch = line[0];
    } else if (ch == '*') {
        bool note = true;
        while (note && !is_end) {
            next_ch();
            if (ch == '*') {
                next_ch();
                if (ch == '/') {
                    note = false;
                    next_ch();
                }
            }
        }
    }
}

void save(string str, Symbol sym) {
    if (sym == DEFAULT) sym = to_sym[str];
    if (sym == DEFAULT) return; //delete NUL at the end of file
    store.append(get_sym[sym] + ' ' + str + '\n');
}

void next_ch() {
    if (ch_no == line.size()) {
        if (!getline(in, line)) {
            is_end = true;
        } else {
            line_no++;
            ch_no = 0;
            ch = line[0];
        }
    } else {
        ch = line[++ch_no];
    }
}

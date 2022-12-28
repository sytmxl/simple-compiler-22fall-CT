//#include "opt.h"
#include "mid.h"
bool opt = true;
#define N 5
int to2(string x) {
    int b = to_int(x);
    int num = 0;
    while (!(b % 2)) {
        b = b / 2;
        num++;
        if (b == 1) return num;
    }
    return -1;
}
void peephole() {
    
    for (int i = 0; i < quads.size(); i++) {

        if (quads[i].op == ASSI and quads[i+1].op == ASSI) {
            if (quads[i].x == quads[i+1].y and quads[i].y == quads[i+1].x) {
                quads.erase(quads.begin() + i + 1);
            }
            if (quads[i].x == quads[i+1].x and quads[i].y == quads[i+1].y) {
                quads.erase(quads.begin() + i + 1);
            }
        }
//        else if (quads[i].op == DIRECT) {
//            string jump = split(quads[i].x, " ")[0];
//            if (jump == "beqz" or jump == "bnez" or jump == "j" or jump == "blez") {
//                string label = split(quads[i].x, " ")[2];
//                if (label.empty()) label = split(quads[i].x, " ")[1];
//                cout << label <<endl;
//                cout << quads[i].x <<endl;
//                int j=i;
//                while (++j < quads.size() and quads[j].op == LABEL)
//                    if (quads[j].x == label)
//                        quads.erase(quads.begin() + i);
//            }
//        }
        else if (quads[i].op == ASSI and quads[i].y == "0") {
            if (quads[i+1].op == POP_STACK) {

            }
        }
        else if (quads[i].op == _MUL) {
            if (to2(quads[i].y) != -1) {
                quads[i].op = DIRECT;
                quads[i].x = "sll "+quads[i].z+", "+quads[i].x+", "+ to_string(to2(quads[i].y));
            }
        }
    }
    for (int i = 0; i < quads.size(); i++) {//breaking structure
        if (quads[i].op == PUSH_STACK and quads[i+1].op == POP_STACK){
            quads[i].op = DIRECT;
            quads[i].x = "move $t0, $t1";
            quads.erase(quads.begin() + i + 1);
        }
    }
};
void optmize() {
    peephole();
}

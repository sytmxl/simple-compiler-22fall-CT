#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mips.h"
#include "mid.h"
int ga = 10;
int gb = -1024;
const int gc = 10 + -1024;

void publish() {
    printf("call publish and do nothing\n");
}

void readGa() {
    ga = 19;
    printf("read %d into ga\n",ga);
}

int add(int a, int b) {
    int r = a + b;
    return r;
}

int sub(int a, int b) {
    a = a - b;
    return a;
}

void addGaGb(int n) {
    ga = ga + n;
    gb = gb + n;
}

int sum(int a, int b, int c, int d, int e, int f, int g) {
    int r = a + b + c + d + e + f + g;
    return r;
}
void print2 (int x[][4]) {
    int i=0, j=0;
    printf("test:%d\n",x[1][0]);
    printf("x2:\n");

    while (i < 3) {
        while (j < 4) {
            printf("%d ", x[i][j]);
            j=j+1;
        }
        j = 0;
        i=i+1;
        printf("\n");
    }
}
void print (int x[][4],int y[]) {
    int i=0, j=0;

    print2(x);
    printf("y:\n");
    i=0; j=0;
        while (j < 4) {
            printf("%d ", y[j]);
            j=j+1;
        }
        printf("\n");

}

int main() {
    new_tab();
    mips << ".data" << endl;
    CompUnit();
    set();
//    print_error();
    vector<Quadruple>::iterator ite = quads.begin();
    insert_tab("str0", SymEntry(I_STR,1,1, 1));
    quads.insert(ite, 1, Quadruple(STR, "str0", "\\n", ""));
    print_quad();
    print_mips();
    mips.close();
    out.close();




    int x[3][4] = {{1,3,4,2},{5,7,8,6}, {9,2,5,3}};
    int y[3][4] = {{1,0,4,0},{5,7,8,6}, {9,2,5,3}};
    int i=0, j=0;
    printf("x1:\n");
    while (i < 3) {
        while (j < 4) {
            printf("%d ", x[i][j]);
            j=j+1;
        }
        j = 0;
        i=i+1;
        printf("\n");
    }
    print(x, y[2]);
    if(-1 && -1) printf("-1\n");

    return 0;
}


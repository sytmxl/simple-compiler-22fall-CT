#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mips.h"
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
int main() {
    new_tab();
    mips << ".data" << endl;
    CompUnit();
    set();
//    print_error();
    vector<Quadruple>::iterator ite = quads.begin();
    insert_tab("str0", SymEntry(I_STR,1,1,1));
    quads.insert(ite, 1, Quadruple(STR, "str0", "\\n", ""));
    print_quad();
    print_mips();
    mips.close();
    out.close();




    int a = add(add(10,20), sub(100,10));
    int b = sub(add(ga,gc) + add(10,5)/3, sub(100,add(a,30)));
    printf("5: %d %d\n",a,b);
    a = 1+sub(100,10);
    b = add(ga,gc) + add(10,5)/3;
    printf("%d %d\n",a,b);
//    int aa = add(ga,gc);
//    int bb = add(10,5);
//    printf("5.5: %d %d\n",aa,bb);
//    aa = add(ga,gc) + add(10,5);
//    bb = sub(100,add(a,30));
//    printf("5.6: %d %d\n",aa,bb);
    int t_1 = 928;
    int t1 = sum(294, -+-+-t_1, sub(a, 10), ga+gb, add(gc, 10), add(10,sub(t_1, 800)), sum(1,1,1,1,1,1,-1));
    printf("6: %d\n",t1);
    return 0;
}


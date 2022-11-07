#include "sym.h"
#include "phas.h"
#include "err.h"
#include "mips.h"
const int x=1+2, y=5,z = 10;
const int con1=0;
int m = (z+x*y)%x+4;
int n = m*2;
const int ZERO = 0;
const int THREE = -1*3*5+60;
int one = 1, two = 2, three =  55 / 9 - 9 % 6;
int func2(int a, int b,int c,int d,int e,int f) {
    int ok,ojbk,res=20;
    f=1;
    f = d+e;
    res = res/4;

    printf("func2:res = %d\n%d\n", res, f);
    return a +(z+x*y)%x+4+b+9;//a+b+14
}
void func(int a, int b,int c,int d,int e,int f) {
    int ok,ojbk,res=1;
    res = func2(m,res,z,m,res,z);//20
    res = res/4;//5

    printf("func:res = %d\n%d\n", res, func2(m,res,z,m,res,z));
//    return x+y*z;
}
int fun(){
    return 250;
}
void print(int x) {
    printf("%d\n", x);
}
int main() {
    new_tab();
    mips << ".data" << endl;
    CompUnit();
    set();
//    print_error();
    vector<Quadruple>::iterator ite = quads.begin();
    insert_tab("str0", SymEntry(I_STR));
    quads.insert(ite, 1, Quadruple(STR, "str0", "\\n", ""));
    print_quad();
    print_mips();
    mips.close();
    out.close();

    const int a1 = 1;
    const int _bc = -2, _de = 3;
    const int fff = +4, ggg = 5, hhh = 6;
    int x1 = 111;
    int x2 = 123 + 234;
    int x3 = 6 * (5 + 3) - 10;
    int x4;
    x4 = x1 + x2 * 3;
    int x5, x6;
    int x7, x8, x9, x10;

    int y1, y2, y3, y4, y5, y6, y7, y8;

    x5 = 3;
    x6 = 4;
    x7 = 5;
    x8 = 6;
    x9 = 7;
    x10 = 8;

    printf("20373184\n");
    printf("Wow, you will begin test your program!\n");
    printf("%d\n", 1 + 2);
    printf("%d\n", ggg * hhh);

    y1 = (a1 + _bc) * _de;
    y2 = x2 % x1 + ggg;
    y3 = x4 * x3 - fff;
    y4 = x5 / 2 * hhh;
    y5 = x6 * (1 + x7) % 3;
    y6 = -x8;
    y7 = 6 + x9;
    printf("%d\n", y1);
    printf("%d\n", y2);
    printf("%d\n", y3);
    printf("%d\n", y4);
    printf("%d\n", y5);
    printf("%d-%d-%d-%d\n", y6, y7, +-+1,fun()+fun());
    return 0;
}


#ifndef CT_PHAS_H
#define CT_PHAS_H

void error();
void if_not_error(Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void next_not_error(Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void CompUnit();
void Decl();
void FuncDef();
void MainFuncDef();
void ConstDecl();
void VarDecl();
void BType();
void ConstDef();
void Ident();
void ConstExp();
void ConstInitVal();
void VarDef();
void InitVal();
void FuncType();
void FuncFParams();
void Block();
void FuncFParam();
void BlockItem();
void Stmt();
void LVal();
void Exp();
void Cond();
void FormatString();
void UnaryOp();
void Number();
void IntConst();
void FuncRParams();
void AddExp();
void MulExp();
void UnaryExp();
void PrimaryExp();
void LAndExp();
void EqExp();
void RelExp();
void LOrExp();
void exp_loop(void (*func1)(), void (*func2)(), Symbol sym1=DEFAULT, Symbol sym2=DEFAULT, Symbol sym3=DEFAULT, Symbol sym4=DEFAULT);
void print(string str);

void AddExp_print();
void MulExp_print();
void LAndExp_print();
void EqExp_print();
void RelExp_print();
void LOrExp_print();

#endif //CT_PHAS_H

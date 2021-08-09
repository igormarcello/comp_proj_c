#ifndef _CODEGEN_H
#define _CODEGEN_H

void asmLoadConstant(char *s);
void asmLoadVariable(char *s);
void asmNegate();
void asmPush();
void asmPopAdd();
void asmPopSub();
void asmPopOr();
void asmPopXor();
void asmPopMul();
void asmPopDiv();
void asmStoreVariable(char *name);

#endif


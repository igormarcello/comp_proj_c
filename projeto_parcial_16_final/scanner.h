#ifndef _SCANNER_H
#define _SCANNER_H

#define MAXNAME 32
#define MAXNUM 6

int isAddOp(char c);
int isMulOp(char c);

void match(char c);
void getName(char *name);
void getNum(char *num);

#endif


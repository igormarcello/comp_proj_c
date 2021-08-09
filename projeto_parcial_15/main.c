#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define MAXNAME 32
#define MAXNUM 6

int isAddOp(char c);
int isMulOp(char c);

void match(char c);
void getName(char *name);
void getNum(char *num);

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


void error(char *fmt, ...);
void expected(char *fmt, ...);


extern char look;

void initInput();
void nextChar();



void emit(char *s, ...); /* emite uma instru��o */

void factor();
void multiply();
void divide();
void term();
void signedTerm();
void add();
void subtract();
void boolOr();
void boolXor();
void expression();
void assignment();






char look; /* caracter "lookahead */



int main()
{
    char name = 'A';
    error("'%c' is not a variable", name);

    printf("%c", getName());

    match('=');

    printf("%c", getNum());

    match('+');

    printf("%c", getName());

    return 0;
}

/* carrega uma constante no registrador prim�rio */
void asmLoadConstant(char *s)
{
    emit("MOV AX, %s", s);
}

/* carrega uma vari�vel no registrador prim�rio */
void asmLoadVariable(char *s)
{
    emit("MOV AX, [%s]", s);
}

/* inverte sinal de reg. prim. */
void asmNegate()
{
    emit("NEG AX");
}

/* coloca registrador prim�rio na pilha */
void asmPush()
{
    emit("PUSH AX");
}

/* adiciona topo da pilha a reg. prim�rio */
void asmPopAdd()
{
    emit("POP BX");
    emit("ADD AX, BX");
}

/* subtrari do topo da pilha o reg. prim�rio */
void asmPopSub()
{
    emit("POP BX");
    emit("SUB AX, BX");
    asmNegate();
}

/* aplica OU com topo da pilha a reg. prim�rio */
void asmPopOr()
{
    emit("POP BX");
    emit("OR AX, BX");
}

/* aplica OU-exclusivo com topo da pilha a reg. prim�rio */
void asmPopXor()
{
    emit("POP BX");
    emit("XOR AX, BX");
}

/* adiciona topo da pilha a reg. prim�rio */
void asmPopMul()
{
    emit("POP BX");
    emit("IMUL BX");
}

/* subtrari do topo da pilha o reg. prim�rio */
void asmPopDiv()
{
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* aplica AND com topo da pilha a reg. prim�rio */
void asmPopAnd()
{
    emit("POP BX");
    emit("AND AX, BX");
}

/* aplica NOT a reg. prim�rio */
void asmNot()
{
    emit("NOT AX");
}

/* armazena valor do registrador prim�rio em vari�vel */
void asmStoreVariable(char *name)
{
    emit("MOV [%s], AX", name);
}

/* analisa e traduz um fator matem�tico */
void factor()
{
	char name[MAXNAME+1], num[MAXNUM+1];

	if (look == '(') {
		match('(');
		expression();
		match(')');
        } else if (isdigit(look)) {
        	getNum(num);
        	asmLoadConstant(num);
        } else if (isalpha(look)) {
                getName(name);
                asmLoadVariable(name);
        } else
                error("Unrecognized character: '%c'", look);
}

/* analisa e traduz um fator com NOT opcional */
void notFactor()
{
	if (look == '!') {
		match('!');
		factor();
		asmNot();
	} else
		factor();
}

/* analisa e traduz uma opera��o de multiplica��o */
void multiply()
{
	match('*');
	asmPush();
	notFactor();
	asmPopMul();
}

/* analisa e traduz uma opera��o de divis�o */
void divide()
{
	match('/');
	asmPush();
	notFactor();
	asmPopDiv();
}

/* analisa e traduz uma opera��o AND */
void boolAnd()
{
	match('&');
	asmPush();
	notFactor();
	asmPopAnd();
}

/* analisa e traduz um termo */
void term()
{
	notFactor();
	while (isMulOp(look)) {
		switch (look) {
		  case '*':
		  	multiply();
		  	break;
		  case '/':
		  	divide();
		  	break;
		  case '&':
		  	boolAnd();
		  	break;
		}
	}
}

/* analisa e traduz um termo com um sinal opcional */
void signedTerm()
{
	char sign = look;
	if (isAddOp(look))
		nextChar();
	term();
	if (sign == '-')
		asmNegate();
}

/* analisa e traduz uma opera��o de soma */
void add()
{
	match('+');
	asmPush();
	term();
	asmPopAdd();
}

/* analisa e traduz uma opera��o de subtra��o */
void subtract()
{
	match('-');
	asmPush();
	term();
	asmPopSub();
}

/* analisa e traduz uma opera��o OU booleana */
void boolOr()
{
	match('|');
	asmPush();
	term();
	asmPopOr();
}

/* analisa e traduz uma opera��o OU-exclusivo booleana */
void boolXor()
{
	match('~');
	asmPush();
	term();
	asmPopXor();
}

/* analisa e traduz uma express�o */
void expression()
{
	signedTerm();
	while (isAddOp(look)) {
		switch (look) {
		  case '+':
		  	add();
		  	break;
		  case '-':
		  	subtract();
		  	break;
		  case '|':
		  	boolOr();
		  	break;
		  case '~':
		  	boolXor();
		  	break;
		}
	}
}

/* analisa e traduz um comando de atribui��o */
void assignment()
{
	char name[MAXNAME+1];

	getName(name);
	match('=');
	expression();
	asmStoreVariable(name);
}

/* reconhece um operador aditivo */
int isAddOp(char c)
{
	return (c == '+' || c == '-' || c == '|' || c == '~');
}

/* reconhece um operador multiplicativo */
int isMulOp(char c)
{
	return (c == '*' || c == '/' || c == '&');
}

/* verifica se caracter combina com o esperado */
void match(char c)
{
	if (look != c)
		expected("'%c'", c);
	nextChar();
}

/* retorna um identificador */
void getName(char *name)
{
	int i;

	if (!isalpha(look))
		expected("Name");
	for (i = 0; isalnum(look); i++) {
		if (i >= MAXNAME)
			error("Identifier too long.");
		name[i] = toupper(look);
		nextChar();
	}
	name[i] = '\0';
}

/* retorna um n�mero */
void getNum(char *num)
{
	int i;

	if (!isdigit(look))
		expected("Integer");
	for (i = 0; isdigit(look); i++) {
		if (i >= MAXNUM)
			error("Integer too large.");
		num[i] = look;
		nextChar();
	}
	num[i] = '\0';
}

/* l� pr�ximo caracter da entrada */
void nextChar()
{
	look = getchar();
}

/* inicializa o m�dulo de entrada */
void initInput()
{
	nextChar();
}

/* emite uma instru��o seguida por uma nova linha */
void emit(char *fmt, ...)
{
	va_list args;

	putchar('\t');

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	putchar('\n');
}


void error(char *fmt, ...)
{
	va_list args;

	fputs("Error: ", stderr);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fputc('\n', stderr);

	exit(1);
}

void expected(char *fmt, ...)
{
	va_list args;

	fputs("Error: ", stderr);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fputs(" expected!\n", stderr);

	exit(1);
}


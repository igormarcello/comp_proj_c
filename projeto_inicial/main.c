/*
The Cradle - O Ber�o

O c�digo abaixo foi escrito por Felipo Soranz e � uma adapta��o
do c�digo original em Pascal escrito por Jack W. Crenshaw em sua
s�rie "Let's Build a Compiler".

Este c�digo � de livre distribui��o e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* prot�tipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
char getNum();
void emit(char *fmt, ...);

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    expression();

    return 0;
}

/* reconhece operador aditivo */
int isAddOp(char c)
{
        return (c == '+' || c == '-');
}

/* inicializa��o do compilador */
void init()
{
    nextChar();
}

/* l� pr�ximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);

    exit(1);
}

/* alerta sobre alguma entrada esperada */
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

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
}

/* recebe o nome de um identificador */
char getName()
{
    char name;

    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();

    return name;
}

/* recebe um n�mero inteiro */
char getNum()
{
    char num;

    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();

    return num;
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

/* analisa e traduz um fator matem�tico */
void factor()
{
        if (look == '(') {
                match('(');
                expression();
                match(')');
        } else
                emit("MOV AX, %c", getNum());
}

/* reconhece e traduz uma multiplica��o */
void multiply()
{
        match('*');
        factor();
        emit("POP BX");
        emit("IMUL BX");
}

/* reconhece e traduz uma divis�o */
void divide()
{
        match('/');
        factor();
        emit("POP BX");
        emit("XCHG AX, BX");
        emit("CWD");
        emit("IDIV BX");
}

/* analisa e traduz um termo */
void term()
{
        factor();
        while (look == '*' || look == '/') {
                emit("PUSH AX");
                switch(look) {
                  case '*':
                        multiply();
                        break;
                  case '/':
                        divide();
                        break;
                  default:
                        expected("MulOp");
                        break;
                }
        }
}

/* reconhece e traduz uma adi��o */
void add()
{
        match('+');
        term();
        emit("POP BX");
        emit("ADD AX, BX");
}

/* reconhece e traduz uma subtra��o */
void subtract()
{
        match('-');
        term();
        emit("POP BX");
        emit("SUB AX, BX");
        emit("NEG AX");
}

/* reconhece e traduz uma express�o */
void expression()
{
        if (isAddOp(look))
                emit("XOR AX, AX");
        else
                term();
        while (isAddOp(look)) {
                emit("PUSH AX");
                switch(look) {
                  case '+':
                        add();
                        break;
                  case '-':
                        subtract();
                        break;
                  default:
                        expected("AddOp");
                        break;
                }
        }
}



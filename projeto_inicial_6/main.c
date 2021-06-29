/*
The Cradle - O Berço

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
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
    boolExpression();

    return 0;
}

/* inicialização do compilador */
void init()
{
    nextChar();
}

/* lê próximo caracter da entrada */
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

/* recebe um número inteiro */
char getNum()
{
    char num;

    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();

    return num;
}

/* emite uma instrução seguida por uma nova linha */
void emit(char *fmt, ...)
{
    va_list args;

    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}

/* reconhece um operador OU */
int isOrOp(char c)
{
        return (c == '|' || c == '~');
}

/* reconhece uma literal Booleana */
int isBoolean(char c)
{
        return (c == 'T' || c == 'F');
}

/* recebe uma literal Booleana */
int getBoolean()
{
        int boolean;

        if (!isBoolean(look))
                expected("Boolean Literal");
        boolean = (look == 'T');
        nextChar();

        return boolean;
}

/* reconhece e traduz um operador OR */
void boolOr()
{
        match('|');
        boolTerm();
        emit("POP BX");
        emit("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void boolXor()
{
        match('~');
        boolTerm();
        emit("POP BX");
        emit("XOR AX, BX");
}

/* analisa e traduz um fator booleano */
void boolFactor()
{
        if (isBoolean(look)) {
                if (getBoolean())
                        emit("MOV AX, -1");
                else
                        emit("MOV AX, 0");
        } else
                relation();
}

/* analisa e traduz um fator booleno com NOT opcional */
void notFactor()
{
        if (look == '!') {
                match('!');
                boolFactor();
                emit("NOT AX");
        } else
                boolFactor();
}


/* analisa e traduz uma expressão booleana */
void boolExpression()
{
        boolTerm();
        while (isOrOp(look)) {
                emit("PUSH AX");
                switch (look) {
                  case '|':
                        boolOr();
                        break;
                  case '~' :
                        boolXor();
                        break;
                }
        }
}


/* analisa e traduz um termo booleano*/
void boolTerm()
{
        notFactor();
        while (look == '&') {
                emit("PUSH AX");
                match('&');
                notFactor();
                emit("POP BX");
                emit("AND AX, BX");
        }
}


/* analisa e traduz uma relação */
void relation()
{
        emit("# relation");
        nextChar();
}

/* reconhece operadores relacionais */
int isRelOp(char c)
{
        return (c == '=' || c == '#' || c == '<' || c == '>');
}

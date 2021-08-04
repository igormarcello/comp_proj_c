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

#define VARTBL_SZ 26
char vartbl[VARTBL_SZ];

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
int getNum();
void emit(char *fmt, ...);

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    prog();

    if (look != '\n')
        fatal("Unexpected data after \'.\'");
    return 0;;
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
int getNum(){
    int i;
    i = 0;
    if (!isdigit(look))
    expected("Integer");
    while (isdigit(look)) {
        i *= 10;
        i += look - '0';
        nextChar();

    }
        return i;
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

void prog() {

    match('p');
    header();
    topdecls();
    mainblock();
    match('.');

}

void header()
{

    printf(".model small\n");
    printf(".stack\n");
    printf(".code\n");
    printf("PROG segment byte public\n");
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");

}

void prolog()

{

        printf("MAIN:\n");

        printf("\tmov ax, PROG\n");

        printf("\tmov ds, ax\n");

        printf("\tmov es, ax\n");

}

void epilog()

{

        printf("\tmov ax,4C00h\n");

        printf("\tint 21h\n");

        printf("PROG ends\n");

        printf("\tend MAIN\n");

}

void mainblock() {

    match('b');
    prolog();
    match('e');
    epilog();
}

void allocvar(char name)
{
    int value = 0, signal = 1;
    if (look == '=') {
        match('=');
        if (look == '-') {
            match('-');
            signal = -1;
        }
        value = signal * getNum();
    }
    printf("%c:\tdw %d\n", name, value);

}

void decl()
{
    match('v');
    for (;;) {
        allocvar(getName());
        if (look != ',')
        break;
    match(',');

    }

}

void topdecls()
{
    while (look != 'b') {
        switch (look) {
            case 'v':
                decl();
            break;
            default:
            error("Unrecognized keyword.");
            expected("BEGIN");
            break;
        }
    }
}

int intable(char name)
{
    return (vartbl[name - 'A'] != ' ');

}

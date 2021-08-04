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

char cur_class; /* classe atual lida por getclass */
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
    while (look != EOF) {
        getclass();
        gettype();
        topdecl();
    }
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

void prolog(){
    printf(".model small\n");
    printf(".stack\n");
    printf(".code\n");
    printf("PROG segment byte public\n");
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

void epilog(char name){
    printf("exit_prog:\n");
    printf("\tmov ax,4C00h\n");
    printf("\tint 21h\n");
    printf("PROG ends\n");
    printf("\tend %c\n", name);
}

/* analisa e traduz um programa
void prog()
{
    char name;
    match('p');  trata do cabeçalho do programa
    name = getName();
    prolog();
    doblock(name);
    match('.');
    epilog(name);
}*/

/* analisa e traduz um programa Small C */

void prog() {
    while (look != EOF) {
        switch (look) {
            case '#':
                preproc(); break;
            case 'i':
                intdecl(); break;
            case 'c':
                chardecl(); break;
            default:
                dofunction(); break;
        }
    }
}


/* analisa e traduz um bloco pascal */
void doblock(char name)
{
    declarations();
    printf("%c:\n", name);
    statements();

}

void declarations()
{
    int valid;
    do {
            valid = 1;
    switch (look) {
        case 'l':
            labels(); break;
        case 'c':
            constants(); break;
        case 't':
            types(); break;
        case 'v':
            variables(); break;
        case 'p':
            doprocedure(); break;
        case 'f':
            dofunction(); break;
    default:
        valid = 0; break;
            }
    } while (valid);
}

void labels()
{
    match('l');
}

void constants()
{
    match('c');
}

void types()
{
    match('t');
}

void variables()
{
    match('v');
}

void doprocedure()
{
    match('p');
}

void dofunction()
{
    match('f');
}

void statements()
{
    match('b');
    while (look != 'e')
        nextchar();
    match('e');
}

void getclass(){
    if (look == 'a' || look == 'x' || look == 's') {
        cur_class = look;
        nextchar();
        break;
    } else
    cur_class = 'a';
}

void gettype()
{
    cur_type = ' ';
    if (look == 'u') {
        cur_sign = 'u';
        cur_type = 'i';
        nextchar();
    } else {
        cur_sign = 's';
    }
    if (look == 'i' || look == 'l' || look == 'c') {
        cur_type = look;
        nextchar();
    }
}

void topdecl(){
    char name;
    name = getname();
    if (look == '(')
    dofunc(name);
    else
    dodata(name);
}

void dofunc(char name){

    match('(');
    match(')');
    match('{');
    match('}');

    if (cur_type == ' ')
        cur_type = 'i';
            printf("Class: %c, Sign: %c, Type: %c, Function: %c\n",
            cur_class, cur_sign, cur_type, name);
}

void dodata(char name)
{
    if (cur_type == ' ')
    expected("Type declaration");
    for (;;) {
        printf("Class: %c, Sign: %c, Type: %c, Data: %c\n",
        cur_class, cur_sign, cur_type, name);
        if (look != ',')
        break;
        match(',');
        name = getname();

    }
    match(';');
}


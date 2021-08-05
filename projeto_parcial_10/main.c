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
#define TRUE -1
#define FALSE 0

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
    i = 0;
    for (i = 0; i < VARTBL_SZ; i++)
    vartbl[i] = ' ';
    nextchar();

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

void mainblock()
{
    match('b');
    prolog();
    block();
    match('e');
    epilog();

}

void allocvar(char name)
{
    int value = 0, signal = 1;

    if (intable(name)) {
        fprintf(stderr, "Duplicate variable name: %c", name);
        exit(1);
        } else

    vartbl[name - 'A'] = 'v';

    if (look == '=') {
        match('=');
        if (look == '-') {
            match('-');
            signal = -1;
        }
        value = signal * getnum();
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

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name;
    name = getname();
    match('=');
    expression();
    asm_store(name);

}

void block()
{
    while (look != 'e')
    assignment();

}

/* zera o registrador primário */
void asm_clear()
{
    printf("\txor ax, ax\n");

}

/* negativa o reg. primário */
void asm_negative()
{
    printf("\tneg ax\n");

}

/* carrega uma constante numérica no reg. prim. */
void asm_loadconst(int i)
{
    printf("\tmov ax, %d\n", i);

}

/* carrega uma variável no reg. prim. */
void asm_loadvar(char name)
{
    if (!intable(name))
    undefined(name);
    printf("\tmov ax, word ptr %c\n", name);

}

/* coloca reg. prim. na pilha */
void asm_push()
{
    printf("\tpush ax\n");

}

/* adiciona o topo da pilha ao reg. prim. */
void asm_popadd()
{
    rintf("\tpop bx\n");
    printf("\tadd ax, bx\n");

}

/* subtrai o reg. prim. do topo da pilha */
void asm_popsub()
{
    printf("\tpop bx\n");
    printf("\tsub ax, bx\n");
    printf("\tneg ax\n");

}

/* multiplica o topo da pilha pelo reg. prim. */
void asm_popmul()
{
    printf("\tpop bx\n");
    printf("\timul bx\n");

}

/* divide o topo da pilha pelo reg. prim. */
void asm_popdiv()
{
    printf("\tpop bx\n");
    printf("\txchg ax, bx\n");
    printf("\tcwd\n");
    printf("\tidiv bx\n");

}

/* armazena reg. prim. em variável */
void asm_store(char name)
{
    if (!intable(name))
    undefined(name);
    printf("\tmov word ptr bx, ax\n");

}

/* avisa a respeito de um identificador desconhecido */
void undefined(char name)
{
    fprintf(stderr, "Error: Undefined identifier %c\n", name);
    exit(1);

}

/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(') {
        match('(');
        expression();
        match(')');

    } else if (isalpha(look))
    asm_loadvar(getName());
    else
    asm_loadconst(getnum());

}

/* analisa e traduz um fator negativo */
void negfactor()
{
    match('-');
    if (isdigit(look))
    asm_loadconst(-getnum());
    else {
        factor();
        asm_negative();

    }

}

/* analisa e traduz um fator inicial */
void firstfactor()
{
    switch (look) {
        case '+':
        match('+');
        factor();
        break;
        case '-':
        negfactor();
        break;
        default:
        factor();
        break;

    }

}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    asm_popmul();

}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    asm_popdiv();

}

/* código comum usado por "term" e "firstterm" */
void term1()
{
    while (ismulop(look))  {
        asm_push();
        switch (look) {
            case '*':
            multiply();
            break;
            case '/':
            divide();
            break;

        }

    }

}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    term1();

}

/* analisa e traduz um termo inicial */
void firstterm()
{
    firstfactor();
    term1();

}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    asm_popadd();

}

/* reconhece e traduz uma subtração*/
void subtract()
{
    match('-');
    term();
    asm_popsub();

}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstterm();
    while (isaddop(look))  {
        asm_push();
        switch (look) {
            case '+':
            add();
            break;
            case '-':
            subtract();
            break;

        }

    }

}

int isorop(char c){
    return (c == '|' || c == '~');
}

int isrelop(char c)
{
    return (strchr("=#<>", c) != NULL);

}

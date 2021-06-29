/*
Mais express�es: Vari�veis, fun��es, tokens multi-caracter, espa�os...
O c�digo abaixo foi escrito por Felipo Soranz e � uma adapta��o
do c�digo original em Pascal escrito por Jack W. Crenshaw em sua
s�rie "Let's Build a Compiler".
Este c�digo � de livre distribui��o e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define MAXNAME 30
#define MAXNUM 5

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* prot�tipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void skipWhite();
void match(char c);
void getName(char *name);
void getNum(char *num);
void emit(char *fmt, ...);
int isAddOp(char c);
void ident();
void factor();
void multiply();
void divide();
void term();
void add();
void subtract();
void expression();
void assignment();


/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    assignment();
    if (look != '\n')
        expected("NewLine");
    return 0;
}

/* inicializa��o do compilador */
void init()
{
    nextChar();
    skipWhite();
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

/* pula caracteres de espa�o */
void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
    skipWhite();
}

/* recebe o nome de um identificador */
void getName(char *name)
{
    int i;
    if (!isalpha(look))
        expected("Name");
    for (i = 0; isalnum(look); i++)
    {
        if (i >= MAXNAME)
            fatal("Identifier too long!");
        name[i] = toupper(look);
        nextChar();
    }
    name[i] = '\0';
    skipWhite();
}

/* recebe um n�mero inteiro */
void getNum(char *num)
{
    int i;
    if (!isdigit(look))
        expected("Integer");
    for (i = 0; isdigit(look); i++)
    {
        if (i >= MAXNUM)
            fatal("Integer too long!");
        num[i] = look;
        nextChar();
    }
    num[i] = '\0';
    skipWhite();
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

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* analisa e traduz um identificador */
void ident()
{
    char name[MAXNAME+1];
    getName(name);
    if (look == '(')
    {
        match('(');
        match(')');
        emit("CALL %s", name);
    }
    else
        emit("MOV AX, [%s]", name);
}

/* analisa e traduz um fator */
void factor()
{
    char num[MAXNUM+1];
    if (look == '(')
    {
        match('(');
        expression();
        match(')');
    }
    else if(isalpha(look))
    {
        ident();
    }
    else
    {
        getNum(num);
        emit("MOV AX, %s", num);
    }
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
    while (look == '*' || look == '/')
    {
        emit("PUSH AX");
        switch(look)
        {
        case '*':
            multiply();
            break;
        case '/':
            divide();
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

/* analisa e traduz uma express�o */
void expression()
{
    if (isAddOp(look))
        emit("XOR AX, AX");
    else
        term();
    while (isAddOp(look))
    {
        emit("PUSH AX");
        switch(look)
        {
        case '+':
            add();
            break;
        case '-':
            subtract();
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
    emit("MOV [%s], AX", name);
}

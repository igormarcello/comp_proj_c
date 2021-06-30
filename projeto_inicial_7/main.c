#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#define MAXNAME 30
#define MAXNUM 5
#define MAXTOKEN 30
#define MAXOP 3
#define SYMTBL_SZ 1000
#define KWLIST_SZ 4


/* protótipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
void getName();
void getNum();
void emit(char *fmt, ...);
void skipWhite();
void scan();
void newLine();
int isOp(char c);
void getOp();
void skipComma();
int lookup(char *s, char *list[], int size);
int token;
char look; /* O caracter lido "antecipadamente" (lookahead) */
char value[MAXTOKEN+1];
char *symtbl[SYMTBL_SZ];
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "END"};
const char *kwcode = "ilee";

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

/* Recebe o nome de um identificador */
void getName()
{
    int i, kw;

    if (!isalpha(look))
        expected("Name");

    for (i = 0; isalnum(look) && i < MAXNAME; i++)
    {
        value[i] = toupper(look);
        nextChar();
    }

    value[i] = '\0';
    kw = lookup(value, kwlist, KWLIST_SZ);

    if (kw == -1)
        token = 'x';

    else
        token = kwcode[kw];
}

/* Recebe um número inteiro */
void getNum()
{
    int i;

    if (!isdigit(look))
        expected("Integer");

    for (i = 0; isdigit(look) && i < MAXNUM; i++)
    {
        value[i] = look;
        nextChar();
    }

    value[i] = '\0';
    token = '#';
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


/* pula caracteres de espaço */
void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();
}

void scan()
{
    int kw;

    while (look == '\n')
        newLine();

    if (isalpha(look))
        getName();
    else if (isdigit(look))
        getNum();
    else if (isOp(look))
        getOp();
    else
    {
        value[0] = look;
        value[1] = '\0';
        token = '?';
        nextChar();
    }
    skipWhite();
}

/* reconhece uma linha em branco */
void newLine()
{
    if (look == '\n')
        nextChar();
}

int isOp(char c)

{

    return (strchr("+-*/<>:=", c) != NULL);
}

void getOp()
{
    int i;

    if (!isOp(look))
        expected("Operator");

    for (i = 0; isOp(look) && i < MAXOP; i++)
    {
        value[i] = look;
        nextChar();
    }
    value[i] = '\0';

    if (strlen(value) == 1)
        token = value[0];
    else
        token = '?';
}

void skipComma()
{
    skipWhite();

    if (look == ',')
    {
        nextChar();
        skipWhite();
    }
}

int lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    do
    {
        scan();
        switch (token)
        {
        case 'x':
            printf("Ident: ");
            break;
        case '#':
            printf("Number: ");
            break;
        case 'i':
        case 'l':
        case 'e':
            printf("Keyword: ");
            break;
        default:
            printf("Operator: ");
            break;
        }
        printf("%s\n", value);
        if (value[0] == '\n')
            newLine();
    }
    while (strcmp(value, "END") == 1);

    return 0;
}

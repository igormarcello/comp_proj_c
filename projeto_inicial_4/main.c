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

#define MAXVAR 26
int var[MAXVAR];

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
        do {
                switch (look) {
                  case '?':
                        input();
                        break;
                  case '!':
                        output();
                        break;
                  default:
                        assignment();
                        break;
                }
                newLine();
        } while (look != '.');

        return 0;
}

/* inicialização do compilador */
void init()
{
    nextChar();
    initVar();
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
int getNum()
{
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

/* avalia o resultado de uma expressão */
void expression()
{
        int val;

        if (isAddOp(look))
                val = 0;
        else
                val = term();

        while (isAddOp(look)) {
                switch (look) {
                  case '+':
                        match('+');
                        val += term();
                        break;
                  case '-':
                        match('-');
                        val -= term();
                        break;
                }
        }

        return val;
}

/* avalia um termo */
int term()
{
        int val;

        val = factor();
        while (isMulOp(look)) {
                switch (look) {
                  case '*':
                        match('*');
                        val *= factor();
                        break;
                  case '/':
                        match('/');
                        val /= factor();
                        break;
                }
        }

        return val;
}

/* avalia um fator */
int factor()
{
        int val;

        if (look == '(') {
                match('(');
                val = expression();
                match(')');
        } else if (isalpha(look))
                val = var[getName() - 'A'];
        else
                val = getNum();

        return val;
}

/* inicializa variáveis */
void initVar()
{
        int i;

        for (i = 0; i < MAXVAR; i++)
                var[i] = 0;
}


/* avalia um comando de atribuição */
void assignment()
{
        char name;

        name = getName();
        match('=');
        var[name - 'A'] = expression();
}

/* captura um caracter de nova linha */
void newLine()
{
        if (look == '\n')
                nextChar();
}

/* interpreta um comando de entrada */
void input()
{
        char name;
        char buffer[20];

        match('?');
        name = getName();
        printf("%c? ", name);
        fgets(buffer, 20, stdin);
        var[name - 'A'] = atoi(buffer);
}

/* interpreta um comando de saída */
void output()
{
        char name;

        match("!");
        name = getName();
        printf("%c -> %d", name, var[name - 'A']);
}

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
int labelCount; /* Contador usado pelo gerador de rótulos */

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
        program();

        return 0;
}


/* inicialização do compilador */
void init()
{
    nextChar(0);
    newLabel();

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

/* reconhece e traduz um comando qualquer */
void other()
{
        emit("# %c", getName());
}

/* analisa e traduz um programa completo */
void program()
{
        block();
        if (look != 'e')
                expected("End");
        emit("END");
}

/* analisa e traduz um bloco de comandos */
void block()
{
        int follow;

        follow = 0;

        while (!follow) {
                switch (look) {
                   case 'i':
                        doIf();
                        break;
                   case 'w':
                        doWhile();
                        break;
                   case 'p':
                        doLoop();
                        break;
                   case 'r':
                        doRepeat();
                        break;
                   case 'e':
                   case 'l':
                   case 'u':
                        follow = 1;
                        break;
                    case 'f':
                        doFor();
                        break;
                   default:
                        other();
                        break;
                }
        }
}

/* gera um novo rótulo único */
int newLabel()
{
        return labelCount++;
}

/* emite um rótulo */
int postLabel(int lbl)
{
        printf("L%d:\n", lbl);
}

/* analisa e traduz um comando IF */
void doIf()
{
        int l1, l2;

        match('i');
        condition();
        l1 = newLabel();
        l2 = l1;
        emit("JZ L%d", l1);
        block();
        if (look == 'l') {
                match('l');
                l2 = newLabel();
                emit("JMP L%d", l2);
                postLabel(l1);
                block();
        }
        match('e');
        postLabel(l2);
}

/* analisa e traduz uma condição */
void condition()
{
        emit("# condition");
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
        int l1, l2;

        match('w');
        l1 = newLabel();
        l2 = newLabel();
        postLabel(l1);
        condition();
        emit("JZ L%d", l2);
        block();
        match('e');
        emit("JMP L%d", l1);
        postLabel(l2);
}

/* analisa e traduz um comando LOOP */
void doLoop()
{
        int l;

        match('p');
        l = newLabel();
        postLabel(l);
        block();
        match('e');
        emit("JMP L%d", l);
}

/* analisa e traduz um REPEAT-UNTIL*/
void doRepeat()
{
        int l;

        match('r');
        l = newLabel();
        postLabel(l);
        block();
        match('u');
        condition();
        emit("JZ L%d", l);
}

/* analisa e traduz um comando FOR*/
void doFor()
{
        int l1, l2;
        char name;

        match('f');
        l1 = newLabel();
        l2 = newLabel();
        name = getName();
        match('=');
        expression();
        emit("DEC AX");
        emit("MOV [%c], AX", name);
        expression();
        emit("PUSH AX");
        postLabel(l1);
        emit("MOV AX, [%c]", name);
        emit("INC AX");
        emit("MOV [%c], AX", name);
        emit("POP BX");
        emit("PUSH BX");
        emit("CMP AX, BX");
        emit("JG L%d", l2);
        block();
        match('e');
        emit("JMP L%d", l1);
        postLabel(l2);
        emit("POP AX");
}

void expression()
{
        emit("# EXPR");
}





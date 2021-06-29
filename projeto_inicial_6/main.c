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
    boolExpression();

    return 0;
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


/* analisa e traduz uma express�o booleana */
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


/* analisa e traduz uma rela��o */
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

/* reconhece e traduz um operador de igualdade */
void equals()
{
        int l1, l2;

        match('=');
        l1 = newLabel();
        l2 = newLabel();
        expression();
        emit("POP BX");
        emit("CMP BX, AX");
        emit("JE L%d", l1);
        emit("MOV AX, 0");
        emit("JMP L%d", l2);
        postLabel(l1);
        emit("MOV AX, -1");
        postLabel(l2);
}

/* reconhece e traduz um operador de n�o-igualdade */
void notEquals()
{
        int l1, l2;

        match('#');
        l1 = newLabel();
        l2 = newLabel();
        expression();
        emit("POP BX");
        emit("CMP BX, AX");
        emit("JNE L%d", l1);
        emit("MOV AX, 0");
        emit("JMP L%d", l2);
        postLabel(l1);
        emit("MOV AX, -1");
        postLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void greater()
{
        int l1, l2;

        match('>');
        l1 = newLabel();
        l2 = newLabel();
        expression();
        emit("POP BX");
        emit("CMP BX, AX");
        emit("JG L%d", l1);
        emit("MOV AX, 0");
        emit("JMP L%d", l2);
        postLabel(l1);
        emit("MOV AX, -1");
        postLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void less()
{
        int l1, l2;

        match('<');
        l1 = newLabel();
        l2 = newLabel();
        expression();
        emit("POP BX");
        emit("CMP BX, AX");
        emit("JL L%d", l1);
        emit("MOV AX, 0");
        emit("JMP L%d", l2);
        postLabel(l1);
        emit("MOV AX, -1");
        postLabel(l2);
}

/* analisa e traduz uma rela��o */
void relation()
{
        expression();
        if (isRelOp(look)) {
                emit("PUSH AX");
                switch (look) {
                  case '=':
                        equals();
                        break;
                  case '#':
                        notEquals();
                        break;
                  case '>':
                        greater();
                        break;
                  case '<':
                        less();
                        break;
                }
        }
}


/* analisa e traduz um identificador */
void ident()
{
        char name;

        name = getName();
        if (look == '(') {
                match('(');
                match(')');
                emit("CALL %c", name);
        } else
                emit("MOV AX, [%c]", name);
}

/* analisa e traduz um fator matem�tico */
void factor()
{

        if (look == '(') {
                match('(');
                boolExpression();
                match(')');
        } else if(isalpha(look))
                ident();
        else
                emit("MOV AX, %c", getNum());
}

/* analisa e traduz um fator com sinal opcional */
void signedFactor()
{
        if (look == '+')
                nextChar();
        if (look == '-') {
                nextChar();
                if (isdigit(look))
                        emit("MOV AX, -%c", getNum());
                else {
                        factor();
                        emit("NEG AX");
                }
        } else
                factor();
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

/* analisa e traduz um termo matem�tico */
void term()
{
        signedFactor();
        while (isMulOp(look)) {
                emit("PUSH AX");
                switch(look) {
                  case '*':
                        multiply();
                        break;
                  case '/':
                        divide();
                        break;
                }
        }
}

/* reconhece e traduz uma soma */
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

/* analisa e traduz uma express�o matem�tica */
void expression()
{
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
                }
        }
}

/* reconhece uma linha em branco */
void newLine()
{
        if (look == '\n')
                nextChar();
}

/* analisa e traduz um bloco de comandos */
void block(int exitLabel)
{
        int follow;

        follow = 0;

        while (!follow) {
                newLine();
                switch (look) {
                  case 'i':
                        doIf(exitLabel); break;
                  case 'w':
                        doWhile(); break;
                  case 'p':
                        doLoop(); break;
                  case 'r':
                        doRepeat(); break;
                  case 'f':
                        doFor(); break;
                  case 'd':
                        doDo(); break;
                  case 'b':
                        doBreak(exitLabel); break;
                  case 'e':
                  case 'l':
                  case 'u':
                        follow = 1;
                        break;
                  default:
                        other(); break;
                }
                newLine();
        }
}

/* analisa e traduz um comando de atribui��o */
void assignment()
{
        char name;

        name = getName();
        match('=');
        boolExpression();
        emit("MOV [%c], AX", name);
}

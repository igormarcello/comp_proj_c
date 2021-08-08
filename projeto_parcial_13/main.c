#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SYMTBL_SZ 26

char look; /* O caracter lido "antecipadamente" (lookahead) */

char symtbl[SYMTBL_SZ]; /* tabela de símbolos */

/* rotinas utilitárias */

void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void undefined(char name);
void duplicated(char name);
void unrecognized(char name);
void notvar(char name);

/* tratamento da tabela de símbolos */

char symtype(char name);
char intable(char name);
void addsymbol(char name, char type);
void checkvar(char name);

/* analisador léxico rudimentar */

int isaddop(char c);
int ismulop(char c);
void skipwhite();
void newline();
void match(char c);
char getname();
char getnum();

/* geração de código */

void asm_loadvar(char name);
void asm_storevar(char name);
void asm_allocvar(char name);

/* analisador sintático */

void expression();
void assignment();
void doblock();
void beginblock();
void decl();
void topdecls();

/* analiza e traduz o bloco principal do programa */

void domain()
{
    char name;

    match('P');
    name = getname();
    newline();

    if (intable(name))
        duplicated(n);
    prolog();
    beginblock();

}

/* analiza e traduz as declarações globais */

void topdecls()
{
    while (look != '.') {
            switch (look) {
                case 'v': decl(); break;
                case 'p': doproc(); break;
                case 'P': domain(); break;

                default: unrecognized(look); break;
                }
                newline();
        }
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    header();
    topdecls();
    epilog();
    return 0;

}



/* inicialização do compilador */

void init()

{

        int i;

        for (i = 0; i < SYMTBL_SZ; i++)

                symtbl[i] = ' ';

        nextchar();

        skipwhite();

}

/* lê próximo caracter da entrada em lookahead */

void nextchar()

{

                look = getchar();

}

/* imprime mensagem de erro sem sair */

void error(char *s)

{

                fprintf(stderr, "Error: %s\n", s);

}

/* imprime mensagem de erro e sai */

void fatal(char *s)

{

                error(s);

                exit(1);

}

/* mensagem de erro para string esperada */

void expected(char *s)

{

                fprintf(stderr, "Error: %s expected\n", s);

                exit(1);

}

/* avisa a respeito de um identificador desconhecido */

void undefined(char name)

{

                fprintf(stderr, "Error: Undefined identifier %c\n", name);

                exit(1);

}

/* avisa a respeito de um identificador desconhecido */

void duplicated(char name)

{

                fprintf(stderr, "Error: Duplicated identifier %c\n", name);

                exit(1);

}

/* avisa a respeito de uma palavra-chave desconhecida */

void unrecognized(char name)

{

        fprintf(stderr, "Error: Unrecognized keyword %c\n", name);

        exit(1);

}

/* avisa a respeito de um identificador que não é variável */

void notvar(char name)

{

                fprintf(stderr, "Error: %c is not a variable\n", name);

                exit(1);

}

/* retorna o tipo de um identificador */

char symtype(char name)

{

        return symtbl[name - 'A'];

}

/* verifica se "name" consta na tabela de símbolos */

char intable(char name)

{

        return (symtbl[name - 'A'] != ' ');

}

/* adiciona novo identificador à tabela de símbolos */

void addsymbol(char name, char type)

{

        if (intable(name))

                duplicated(name);

        symtbl[name - 'A'] = type;

}

/* verifica se identificador é variável */

void checkvar(char name)

{

        if (!intable(name))

                undefined(name);

        if (symtype(name) != 'v')

                notvar(name);

}

/* testa operadores de adição */

int isaddop(char c)

{

                return (c == '+' || c == '-');

}

/* testa operadores de multiplicação */

int ismulop(char c)

{

                return (c == '*' || c == '/');

}

/* testa operadores OU */

int isorop(char c)

{

                return (c == '|' || c == '~');

}

/* testa operadores relacionais */

int isrelop(char c)

{

                return (strchr("=#<>", c) != NULL);

}

/* pula caracteres em branco */

void skipwhite()

{

                while (look == ' ' || look == '\t')

                               nextchar();

}

/* reconhece uma quebra de linha */

void newline()

{

        if (look == '\n')

                nextchar();

}

/* verifica se look combina com caracter esperado */

void match(char c)

{

                char s[2];

                if (look == c)

                               nextchar();

                else {

                               s[0] = c; /* uma conversao rápida (e feia) */

                               s[1] = '\0'; /* de um caracter em string */

                               expected(s);

                }

        skipwhite();

}

/* analisa e traduz um nome (identificador ou palavra-chave) */

char getname()

{

                char name;

                if (!isalpha(look))

                               expected("Name");

                name = toupper(look);

                nextchar();

        skipwhite();

                return name;

}

/* analisa e traduz um número inteiro */

char getnum()

{

                char num;

                if (!isdigit(look))

                               expected("Integer");

                num = look;

                nextchar();

        skipwhite();

        return num;

}

/* carrega uma variável no reg. prim. */

void asm_loadvar(char name)

{

        checkvar(name);

                printf("\tmov ax, word ptr %c\n", name);

}

/* armazena reg. prim. em variável */

void asm_storevar(char name)

{

                printf("\tmov word ptr %c, ax\n", name);

}

/* aloca espaço de armazenamento para variável */

void asm_allocvar(char name)

{

        if (intable(name))

                duplicated(name);

        addsymbol(name, 'v');

        printf("%c: dw 0\n", name);

}

/* analisa e traduz uma expressão */

void expression()

{

        asm_loadvar(getname());

}

/* analisa e traduz um comando de atribuição */

void assignment()

{

        char name;

        name = getname();

        match('=');

        expression();

        asm_storevar(name);

}

/* analiza e traduz um bloco de comandos */

void doblock()

{

        while (look != 'e') {

                assignment();

                newline();

        }

}

/* analiza e traduz um bloco begin */

void beginblock()

{

        match('b');

        newline();

        doblock();

        match('e');

        newline();

}

/* analiza e traduz a declaração de uma variável */

void decl()

{

        match('v');

        asm_allocvar(getname());

}

/* analiza e traduz as declarações globais */

void topdecls()
{
    while (look != 'b') {
            switch (look) {
                case 'v': decl(); break;
                case 'p': doproc(); break;
                default: unrecognized(look); break;
                }
                newline();
            }
}

/* analisa e traduz uma declaração de procedimento */
void doproc()
{

                char name;
                match('p');
                name = getname();
                newline();

                if (intable(name))
                               duplicated(n);
                addsymbol(name, 'p');
                printf("%c:\n", name);
                beginblock();
                asm_return();
}

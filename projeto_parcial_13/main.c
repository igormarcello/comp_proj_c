#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#define SYMTBL_SZ 26

char look; /* O caracter lido "antecipadamente" (lookahead) */

char symtbl[SYMTBL_SZ]; /* tabela de s�mbolos */

#define PARAMS_SZ 26

int params[PARAMS_SZ]; /* lista de par�metros formais para os procedimentos */

int nparams; /* n�mero de par�metros formais */

int base;

/* rotinas utilit�rias */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void undefined(char name);
void duplicated(char name);
void unrecognized(char name);
void notvar(char name);

/* tratamento da tabela de s�mbolos */
char symtype(char name);
char intable(char name);
void addsymbol(char name, char type);
void checkvar(char name);

/* analisador l�xico rudimentar */
int isaddop(char c);
int ismulop(char c);
void skipwhite();
void newline();
void match(char c);
char getname();
char getnum();

/* gera��o de c�digo */
void asm_loadvar(char name);
void asm_storevar(char name);
void asm_allocvar(char name);
void header();/**/
void prolog();/**/
void epilog();/**/

/* analisador sint�tico */
void expression();
void assignment(char name);
void doblock();
void beginblock();
void decl();
void topdecls();

/* procedimentos */
void doproc();
void asm_call(char name);
void formallist();
void formalparam();
void param();
int paramlist();
void docallproc(char name);
void domain();
void assign_or_call();
void clearparams();
int paramnum(char name);
int isparam(char name);
void addparam(char name);
int asm_offsetpar(int par);
void asm_loadparam(int par);
void asm_storeparam(int par);
void asm_push();
void asm_cleanstack(int bytes);
void asm_procprolog(char name, int nlocals);
void asm_procepilog();
void locdecl();
int locdecls();
void asm_pushparam(char name);

void asm_return();// n�o implementada

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    header();
    topdecls();
    epilog();

    return 0;
}


/* inicializa��o do compilador */
void init()
{
    int i;

    for (i = 0; i < SYMTBL_SZ; i++)
        symtbl[i] = ' ';

    clearparams();
    nextchar();
    skipwhite();
}

/* l� pr�ximo caracter da entrada em lookahead */
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

/* avisa a respeito de um identificador que n�o � vari�vel */
void notvar(char name)
{
    fprintf(stderr, "Error: %c is not a variable\n", name);
    exit(1);
}

/* retorna o tipo de um identificador */
char symtype(char name)
{
    if (isparam(name))
        return 'f';

    return symtbl[name - 'A'];
}

/* verifica se "name" consta na tabela de s�mbolos */
char intable(char name)
{
    return (symtbl[name - 'A'] != ' ');
}

/* adiciona novo identificador � tabela de s�mbolos */
void addsymbol(char name, char type)
{
    if (intable(name))
        duplicated(name);

    symtbl[name - 'A'] = type;
}

/* verifica se identificador � vari�vel */
void checkvar(char name)
{
    if (!intable(name))
        undefined(name);
    if (symtype(name) != 'v')
        notvar(name);
}

/* testa operadores de adi��o */
int isaddop(char c)
{
    return (c == '+' || c == '-');
}

/* testa operadores de multiplica��o */
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
    else
    {
        s[0] = c;    /* uma conversao r�pida (e feia) */
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

/* analisa e traduz um n�mero inteiro */
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

/* carrega uma vari�vel no reg. prim. */
void asm_loadvar(char name)
{
    checkvar(name);
    printf("\tmov ax, word ptr %c\n", name);
}

/* armazena reg. prim. em vari�vel */
void asm_storevar(char name)
{
    printf("\tmov word ptr %c, ax\n", name);
}

/* aloca espa�o de armazenamento para vari�vel */
void asm_allocvar(char name)
{

    if (intable(name))
        duplicated(name);

    addsymbol(name, 'v');
    printf("%c: dw 0\n", name);
}

/* analisa e traduz uma express�o */
void expression()
{
    char name = getname();

    if (isparam(name))
        asm_loadparam(paramnum(name));
    else
        asm_loadvar(name);
}

/* analisa e traduz um comando de atribui��o */
void assignment(char name)
{
    match('=');
    expression();
    asm_storevar(name);
}

/* analiza e traduz um bloco de comandos */
void doblock()
{
    while (look != 'e')
    {
        assign_or_call();
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

/* analiza e traduz a declara��o de uma vari�vel */
void decl()
{
    match('v');
    asm_allocvar(getname());
}

/* analiza e traduz as declara��es globais */
void topdecls()
{
    while (look != '.')
    {
        switch (look)
        {
        case 'v':
            decl();
            break;
        case 'p':
            doproc();
            break;
        case 'P':
            domain();
            break;
        default:
            unrecognized(look);
            break;
        }
        newline();
    }
}

/* analisa e traduz uma declara��o de procedimento */
void doproc()
{
    int nlocals;
    char name;

    match('p');
    name = getname();
    addsymbol(name, 'p');
    formallist();
    nlocals = locdecls();
    asm_procprolog(name, nlocals);
    beginblock();
    asm_procepilog();
    clearparams();
}

/* analiza e traduz o bloco principal do programa */
void domain()
{
    char name;

    match('P');
    name = getname();
    newline();

    if (intable(name))
        duplicated(name);

    prolog();
    beginblock();
}

/*____*/

/* cabe�alho do c�digo assembly */
void header()
{
    printf(".model small\n");
    printf(".stack\n");
    printf(".code\n");
    printf("extrn READ:near, WRITE:near\n");
    printf("PROG segment byte public\n");
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

/* pr�logo da rotina principal */
void prolog()
{
    printf("MAIN:\n");
    printf("\tmov ax, PROG\n");
    printf("\tmov ds, ax\n");
    printf("\tmov es, ax\n");
}

/* ep�logo da rotina principal */
void epilog()
{
    printf("\tmov ax,4C00h\n");
    printf("\tint 21h\n");
    printf("PROG ends\n");
    printf("\tend MAIN\n");
}

/* analisa e traduz um comando de atribui��o ou chamada de procedimento */
void assign_or_call()
{
    char name;

    name = getname();

    switch (symtype(name))
    {
    case ' ':
        undefined(name);
        break;
    case 'v':
    case 'f':
        assignment(name);
        break;
    case 'p':
        docallproc(name);
        break;
    default:
        printf("Identifier %c cannot be used here!", name);
        exit(1);
        break;
    }
}

/* gera uma chamada de procedimento */
void asm_call(char name)
{
    printf("\tcall %c\n", name);
}

/* processa a lista de par�metros formais de um procedimento */
void formallist()
{
    match('(');

    if (look != ')')
    {
        formalparam();
        while (look == ',')
        {
            match(',');
            formalparam();
        }
    }

    match(')');
    newline();
    base = nparams;
    nparams += 2;
}

/* processa um par�metro formal */
void formalparam()
{
    addparam(getname());
}

/* processa um par�metro de chamada */
void param()
{
    char name = getname();
    asm_pushparam(name);
}

/* processa a lista de par�metros para uma chamada de procedimento */
int paramlist()
{
    int i = 0;

    match('(');

    if (look != ')')
    {
        for (;;)
        {
            param();
            i++;

            if (look != ',')
                break;

            match(',');
        }
    }
    match(')');

    return i * 2; /* n�mero de par�metros * bytes por par�metro */
}

/* processa uma chamada de procedimento */
void docallproc(char name)
{
    int bytes = paramlist();
    asm_call(name);
    asm_cleanstack(bytes);
}

void clearparams()
{
    int i;

    for (i = 0; i < PARAMS_SZ; i++)
        params[i] = 0;

    nparams = 0;
}

/* retorna n�mero indicando a posi��o do par�metro */
int paramnum(char name)
{

    return params[name - 'A'];
}

/* verifica se nome � par�metro */
int isparam(char name)
{
    return (params[name - 'A'] != 0);
}

/* adiciona par�metro � lista */
void addparam(char name)
{
    if (isparam(name))
        duplicated(name);

    params[name - 'A'] = ++nparams;
}

int asm_offsetpar(int par)
{
    int offset;
    /* offset = ret_address + param_size * (base - param_pos) */
    offset = 4 + 2 * (base - par);

    return offset;
}

/* carrega par�metro em registrador prim�rio */
void asm_loadparam(int par)
{
    printf("\tmov ax, word ptr [bp%+d]\n", asm_offsetpar(par));
}

/* armazena conte�do do reg. prim. em par�metro */
void asm_storeparam(int par)
{
    printf("\tmov word ptr [bp%+d], ax\n", asm_offsetpar(par));
}

/* coloca reg. prim. na pilha */
void asm_push()
{
    printf("\tpush ax\n");
}

/* ajusta o ponteiro da pilha acima */
void asm_cleanstack(int bytes)
{

    if (bytes > 0)
        printf("\tadd sp, %d\n", bytes);
}

/* escreve o pr�logo para um procedimento */
void asm_procprolog(char name, int nlocals)
{
    printf("%c:\n", name);
    printf("\tpush bp\n");
    printf("\tmov bp, sp\n");
    printf("\tsub sp, %d\n", nlocals * 2);
}

/* escreve o ep�logo para um procedimento */
void asm_procepilog()
{
    printf("\tmov sp, bp\n");
    printf("\tpop bp\n");
    printf("\tret\n");
}

/* coloca par�metros na pilha */
void asm_pushparam(char name)
{
    switch (symtype(name))
    {
    case 'v':
        printf("\tmov ax, offset %c\n", name);
        asm_push();
        break;
    case 'f':
        printf("\tpush word ptr [bp+%d]\n",
               asm_offsetpar(paramnum(name)));
        break;
    default:
        printf("Identifier %c cannot be used here!", name);
        exit(1);
        break;
    }
}

/* analisa e traduz uma declara��o local de dados */
void locdecl()
{
    match('v');
    addparam(getname());
    newline();
}

/* analisa e traduz declara��es locais */
int locdecls()
{
    int i;

    for (i = 0; look == 'v'; i++)
        locdecl();

    return i;
}

/* gera c�digo para retorno de procedimento */
void asm_return()
{
    printf("\tret\n");
}

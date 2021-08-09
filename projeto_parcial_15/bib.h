#define SYMTBL_SZ 26
char symtbl[SYMTBL_SZ]; /* tabela de símbolos */
char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */

void init();
void nextchar();

void fatal(char *s);

void dumptable();
int isaddop(char c);
int ismulop(char c);
int isorop(char c);
int isrelop(char c);
void skipwhite();
void newline();
void match(char c);
char getname();
char getnum();
char symtype(char name);
char intable(char name);
void duplicated(char name);
void checkdup(char name);
void addsymbol(char name, char type);
void topdecls();
void decl();
void asm_allocvar(char name, char type);
void unrecognized(char name);
void alloc(char name, char type);
void asm_loadvar(char name, char type);
int isvartype(char c);
void notvar(char name);
char vartype(char name);
char loadvar(char name);
void asm_storevar(char name, char type);
void block();
void assignment();
char expression();
void asm_convert(char src, char dst);
void storevar(char name, char srctype);
char loadnum(long val);
void asm_loadconst(long val, char type);
char unop();
void asm_push(char type);
char add(char type);
char subtract(char type);
void asm_pop(char type);
void asm_swap(char type);
char asm_popadd(char t1, char t2);
char asm_popsub(char t1, char t2);
char asm_sametype(char t1, char t2, int ord_matters);
char factor();
char term();
char divide(char type);
char multiply(char type);
char asm_popmul(char t1, char t2);
char asm_popdiv(char t1, char t2);
void asm_clear();

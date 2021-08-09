#include "scanner.h"
#include "parser.h"
#include "codegen.h"
#include "input.h"

/* analisa e traduz um fator matemático */
void factor()
{
    char name[MAXNAME + 1], num[MAXNUM + 1];

    if (isdigit(look))
    {
        getNum(num);
        asmLoadConstant(num);
    }
    else if (isalpha(look))
    {
        getName(name);
        asmLoadVariable(name);
    }
    else
        error("Unrecognized character: '%c'", look);
}

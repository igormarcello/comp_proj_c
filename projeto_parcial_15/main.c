#include <stdio.h>
#include "input.c"
#include "output.c"
#include "errors.c"
#include "scanner1.c"
#include "bib.c"

int main()
{
    char name = 'A';
    error("'%c' is not a variable", name);

    printf("%c", getName());

    match('=');

    printf("%c", getNum());

    match('+');

    printf("%c", getName());

    return 0;
}

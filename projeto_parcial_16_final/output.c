#include <stdio.h>
#include <stdarg.h>
#include "output.h"

/* emite uma instru??o seguida por uma nova linha */
void emit(char *fmt, ...)
{
	va_list args;
	
	putchar('\t');

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	putchar('\n');
}


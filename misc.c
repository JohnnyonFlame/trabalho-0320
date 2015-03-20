#include <stdio.h>

void ler_dados(char *prompt, char *fmt, void *ptr)
{
    do
    {
        fflush(stdin);
        printf("%s", prompt);
    } while (!fscanf(stdin, fmt, ptr));
}

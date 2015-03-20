#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "token.h"
#include "registro.h"
#include "misc.h"

int main(int argc, char *argv[])
{
    char cmd = 0;

    do {
        system("cls");
        printf("i- Inserir registro\n"
               "l- Listar registros\n"
               "d- Deletar registro\n"
               "c- Contar registros\n"
               "e- Sai do programa\n\n"
               "Digite o comando: ");

        fflush(stdin);
        if (!fscanf(stdin, "%c", &cmd))
        {
            cmd = 0; //erro
        }

        switch(tolower(cmd))
        {
        case 'i':
            registro_inserir();
            break;
        case 'l':
            registro_listar();
            break;
        case 'd':
            registro_deleta();
            break;
        case 'c':
            registro_contar();
            break;
        case 'e':
            break;
        default:
            __ERRO("Comando Invalido\n");
            break;
        }
    } while(cmd != 'e');

    return 0;
}

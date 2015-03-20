#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "registro.h"
#include "misc.h"

char *reg_opts[] = {
    "Nome",
    "Vitorias",
    "Classificacao",
    "Peso",
    "Potencia",
    "Pontos",
};

#define n_opts (sizeof(reg_opts) / sizeof(reg_opts[0]))

typedef struct Registro {
    int id;
    char opts[n_opts][60];
} Registro;

static void registro_zera(Registro *reg)
{
    reg->id = 0;
    int i;
    for (i=0; i<n_opts; i++)
        strcpy(reg->opts[i], "N/A");
}

#define f_assert(cond) \
    if ((cond) < 0)\
    { \
        __ERRO("Erro escrevendo em arquivo\n"); \
        return; \
    }

static void registro_escreve(FILE *file, Registro *reg)
{
    f_assert(fprintf(file, "reg %i\n", reg->id));
    int i;
    for (i=0; i<n_opts; i++)
    {
        f_assert(fprintf(file, "\t%s = \"", reg_opts[i]));
        char *c;
        for (c = reg->opts[i]; *c != 0; c++)
        {
            if (*c == '"')
            {
                f_assert(fprintf(file, "\\\""));
            }
            else
                f_assert(fprintf(file, "%c", *c));
        }

        f_assert(fprintf(file, "\"\n"));
    }

    f_assert(fprintf(file, "\n"));
}

#define token_match_or_err(tok_id) \
        if (!token_match(file, &tok, tok_id)) \
        { \
            __ERRO("Erro de sintaxe no arquivo:\n"#tok_id" esperado!\n"); \
            return 0; \
        }

static int registro_ler(FILE *file, Registro *reg)
{
    Token tok;

    if (!reg)
    {
        __ERRO("Ponteiro nulo em registro_ler.\n");
        return 0;
    }

    while (token_readToken(file, &tok))
    {
        //Verifica se recebemos sintaxe: reg <i>
        if ((tok.type == TOKEN_ERR) || (tok.type == TOKEN_EOF))
            return 0;
        if (tok.type == TOKEN_NL)
            continue;
        if ((tok.type != TOKEN_STRING) || (stricmp("reg", tok.str) != 0))
        {
            __ERRO("Erro de sintaxe no arquivo:\nRegistro esperado.\n");
            printf("tok:: %s\n", tok.str);
            return 0;
        }

        //Leia então a ID do nosso registro
        token_match_or_err(TOKEN_STRING);
        if (!sscanf(tok.str, "%i", &reg->id))
        {
            __ERRO("Erro de sintaxe no arquivo:\nEndereco esperado.\n");
            return 0;
        }

        token_match_or_err(TOKEN_NL);
        break;
    }

    while (token_match(file, &tok, TOKEN_STRING))
    {
        int i;
        for (i=0; i<n_opts; i++)
        {
            if (stricmp(tok.str, reg_opts[i]) == 0)
                break;
        }

        token_match_or_err(TOKEN_EQ);
        token_match_or_err(TOKEN_STRING);
        if (strlen(tok.str) > sizeof(reg->opts[0]))
        {
            __ERRO("Opcao possui tamanho grande demais!\n");
            return 0;
        }

        if (i < n_opts)
            strcpy(reg->opts[i], tok.str);

        token_match_or_err(TOKEN_NL);
    }

    if (tok.type == TOKEN_ERR)
        return 0;

    return 1;
}

void registro_listar()
{
    Registro reg;
    FILE *test = fopen("lista.txt", "r");
    registro_zera(&reg);

    if (!test)
    {
        __ERRO("Nao foi possivel abrir lista.txt\n");
        return;
    }

    while(registro_ler(test, &reg))
    {
        system("cls");
        printf("ID/Registro %i:\n\n", reg.id);
        int i;
        for (i=0; i<n_opts; i++)
        {
            printf("%-14s: %s\n", reg_opts[i], reg.opts[i]);
        }
        printf("\n");
        system("pause");

        //Para ter certeza que nao vamos ler lixo na proxima iteracao
        registro_zera(&reg);
    }

    fclose(test);
}

void registro_contar()
{
    Registro reg;
    int contagem = 0;

    FILE *test = fopen("lista.txt", "r");

    if (!test)
    {
        __ERRO("Nao foi possivel abrir lista.txt\n");
        return;
    }

    while(registro_ler(test, &reg))
    {
        contagem++;
    }

    fclose(test);

    system("cls");
    printf("Numero de indices: %i\n", contagem);
    system("pause");
}

void registro_inserir()
{
    system("cls");
    FILE *f = fopen("lista.txt", "a");
    Registro reg;
    int i;

    if (!f)
    {
        __ERRO("Nao foi possivel abrir arquivo lista.txt!\n");
        return;
    }

    ler_dados("ID/Registro: ", "%i", &reg.id);
    printf("\n");

    for (i=0; i<n_opts; i++)
    {
        printf("%-14s: ", reg_opts[i]);

        fflush(stdin);
        fgets(reg.opts[i], sizeof(reg.opts[0]), stdin);

        if (strrchr(reg.opts[i], '\n'))
            *strrchr(reg.opts[i], '\n') = 0;
    }

    registro_escreve(f, &reg);
    fclose(f);
}

void registro_deleta()
{
    FILE *old_f, *new_f;
    Registro reg;
    int i;
    registro_zera(&reg);

    ler_dados("ID do indice a deletar: ", "%i", &i);

    if (rename("lista.txt", "lista_velha.txt") != 0)
    {
        __ERRO("Erro renomeando arquivo de indices.\n");
        return;
    }

    old_f = fopen("lista_velha.txt", "r");
    new_f = fopen("lista.txt", "a");

    if (!old_f)
    {
        __ERRO("Impossivel abrir lista_velha.txt\n");
        return;
    }

    if (!new_f)
    {
        __ERRO("Impossivel abrir lista.txt\n");
        fclose(old_f);
        return;
    }

    while(registro_ler(old_f, &reg))
    {
        if (reg.id != i)
            registro_escreve(new_f, &reg);
        registro_zera(&reg);
    }

    fclose(new_f);
    fclose(old_f);

    unlink("lista_velha.txt");
}

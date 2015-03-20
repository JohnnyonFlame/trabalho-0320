#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	long int reg_size = 0;
	int i;

	reg_size += sizeof(reg_size);
	reg_size += sizeof(reg->id);
	for (i=0; i<n_opts; i++)
	{
		reg_size += sizeof(int);
		reg_size += (strlen(reg->opts[i]) + 1) * sizeof(reg->opts[0][0]);
	}

	fwrite(&reg_size, sizeof(reg_size), 1, file); //stub
	fwrite(&reg->id, sizeof(reg->id), 1, file);

	for (i=0; i<n_opts; i++)
	{
		int write_sz = (strlen(reg->opts[i]) + 1) * sizeof(reg->opts[0][0]);
		fwrite(&write_sz, sizeof(write_sz), 1, file);
		fwrite(reg->opts[i], write_sz, 1, file);
	}
}

#define assert_r(cond) \
	if (!(cond)) \
	{ \
		__ERRO("Indice de registro corrompido!\n"); \
		return 0; \
	}

static int registro_ler(FILE *file, Registro *reg, int seek_out)
{
	if (seek_out)
	{
		long int sz_reg;
		if (!fread(&sz_reg, sizeof(sz_reg), 1, file))
			return 0;

		fseek(file, sz_reg - sizeof(sz_reg), SEEK_CUR);
		return 1;
	}
	fseek(file, sizeof(long int), SEEK_CUR);

	if (!fread(&reg->id, sizeof(reg->id), 1, file))
		return 0;

	int i;
	for (i=0; i<n_opts; i++)
	{
		int read_sz;
		assert_r(fread((void*)&read_sz, sizeof(int), 1, file));
		assert_r(fread((void*)&reg->opts[i][0], read_sz, 1, file));
	}
}

void registro_listar()
{
    Registro reg;
    FILE *test = fopen("lista.dat", "rb");
    registro_zera(&reg);

    if (!test)
    {
        __ERRO("Nao foi possivel abrir lista.dat\n");
        return;
    }

    while(registro_ler(test, &reg, 0))
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

    FILE *test = fopen("lista.dat", "rb");

    if (!test)
    {
        __ERRO("Nao foi possivel abrir lista.dat\n");
        return;
    }

    while(registro_ler(test, &reg, 1))
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
    FILE *f = fopen("lista.dat", "ab");
    Registro reg;
    int i;

    if (!f)
    {
        __ERRO("Nao foi possivel abrir arquivo lista.dat!\n");
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

    if (rename("lista.dat", "lista_velha.dat") != 0)
    {
        __ERRO("Erro renomeando arquivo de indices.\n");
        return;
    }

    old_f = fopen("lista_velha.dat", "r");
    new_f = fopen("lista.dat", "a");

    if (!old_f)
    {
        __ERRO("Impossivel abrir lista_velha.txt\n");
        return;
    }

    if (!new_f)
    {
        __ERRO("Impossivel abrir lista.dat\n");
        fclose(old_f);
        return;
    }

    while(registro_ler(old_f, &reg, 0))
    {
        if (reg.id != i)
            registro_escreve(new_f, &reg);
        registro_zera(&reg);
    }

    fclose(new_f);
    fclose(old_f);

    unlink("lista_velha.dat");
}

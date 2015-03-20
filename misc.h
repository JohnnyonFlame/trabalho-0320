#ifndef __MISC_H__
#define __MISC_H__

#define __ERRO(a) {system("cls"); printf("%s,%i: %s\n", __FILE__, __LINE__, a); system("pause");}

void ler_dados(char *prompt, char *fmt, void *ptr);

#endif //__MISC_H__

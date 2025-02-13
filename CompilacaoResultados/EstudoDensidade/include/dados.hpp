#if !defined(DADOS_H)
#define DADOS_H

#include <string.h>
#include <iostream>
using namespace std;

typedef struct inst{
    string name;
    int n;
    int m;
    float * db;
    double * alfa;
    int sizeArquivos;
}Inst;

typedef struct Dados{
    Inst * inst;
    int sizeInts;
    int capacityInst;
    int qtdArquivos;
} Dados;

Dados * dados_create(int);
void dados_add_new(Dados *, char*, char*, char*, char*, char*);
void dados_add_old(Dados *, char*, char*, char*, char*, char*);
bool dados_exist(Dados *, char*);
void dados_sort(Dados *);

#endif // DADOS_H



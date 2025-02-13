#include "dados.h"

Dados * dados_create(int qtd)
{   
    Dados * d  = (Dados*)malloc(sizeof(Dados));

    d->qtdArquivos = qtd;
    d->capacityInst = 525;
    d->sizeInts = 0; 

    d->inst= (Inst *)malloc(525*sizeof(Inst));
    for(int i = 0; i < 525; i++){
        d->inst[i].db = (float *) calloc(qtd,sizeof(float));
        d->inst[i].time = (double *) calloc(qtd,sizeof(double)); 
        d->inst[i].sizeArquivos = 0;
    }
    return d;
}

void dados_set(Inst * i, char* name, char* n, char* m, char* db, char* time){
    i->name = strdup(name);
    i->n = atof(n);
    i->m = atof(m);
    i->db[i->sizeArquivos] = atof(db);
    i->time[i->sizeArquivos] = atof(time);
    i->sizeArquivos++;


}

void dados_insert(Inst * i, char* db, char* time){
    i->db[i->sizeArquivos] = atof(db);
    i->time[i->sizeArquivos] = atof(time);
    i->sizeArquivos++;
}

void dados_add_new(Dados * d, char* name, char* n, char* m, char* db, char* time){
    if (d->sizeInts >= d->capacityInst)
    {
        d->capacityInst *= 2;
        d->inst = (Inst *)realloc(d->inst, d->capacityInst * sizeof(Inst));
    }

    Inst * i = &(d->inst[d->sizeInts]);
    d->sizeInts++;
    dados_set(i, name, n, m, db, time);
}

void dados_add_old(Dados * d, char* name, char* n, char* m, char* db, char* time)
{
    for(int i = 0; i < d->sizeInts; i++){
        if (d->inst[i].name == name){
            Inst * add = &(d->inst[i]);
            dados_insert(add, db, time);
        }
    }
}

bool dados_exist(Dados * d, char * name)
{
    for(int i = 0; i < d->sizeInts; i++){
        if (d->inst[i].name == name) 
            return true;
    }
    return false;
}

int inst_compare(const void *a, const void *b)
{
    Inst *inst1 = (Inst *)a;
    Inst *inst2 = (Inst *)b;

    float n1 = inst1->n;
    float n2 = inst2->n;

    float m1 = inst1->m;
    float m2 = inst2->m;

    if(n1 == n2){
        if(m1 == m2)
            return 0;
        if(m1 < m2)
            return -1;
        else
            return 1;
    }
    else if(n1 < n2)
        return -1;
    else
        return 1;

}

void dados_sort(Dados * d){
    qsort(d->inst, d->sizeInts, sizeof(Inst), inst_compare);
}

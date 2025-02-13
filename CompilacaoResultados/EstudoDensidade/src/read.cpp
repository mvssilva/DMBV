#include "read.hpp"

using namespace std;

void setup(Dados * d, char * directory)
{
    FILE *file = fopen(directory, "r");
    if (file == NULL){
        exit(1);
    }

    char *line = NULL;
    size_t size = 0;
    int len;

    while ((len = getline(&line, &size, file)) != -1)
    {
        line[len - 1] = '\0';
        process_line(d, line);
    }
}

void process_line(Dados * d, char * line){
    char *file = strrchr(line, '/');
    char *name = strtok(file, ";");
    char * n = strtok(NULL, ";");
    char * m = strtok(NULL, ";");
    char * dbranch = strtok(NULL, ";");
    char *alfa = strtok(NULL, ";");
    // cout << alfa << endl;
    if(!dados_exist(d, name))
        dados_add_new(d, name, n, m, dbranch, alfa);
    else
        dados_add_old(d, name, n, m, dbranch, alfa);

}


void write_customize(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-complete.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; exact; alfa;\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n != naux){

            float soma = 0;
            float qtd = 0;
            int indiceArquivo = 0;

            for(int i = 0; i < d->sizeInts; i++){
                
                if(d->inst[i].n == n){
                    soma += (float)d->inst[i].m;
                    qtd++;
                }
            }
            float mmedio = soma / qtd;


            float * vectordb = (float*) malloc(d->qtdArquivos * sizeof(float));
            double * vectoralfa = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somaalfa = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n){
                        somadb +=  d->inst[i].db[j];
                        somaalfa +=  d->inst[i].alfa[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectoralfa[j] = somaalfa / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.1f;", mmedio);
            fprintf(file, "%.2f; %.4f;", vectordb[0], vectoralfa[0]);
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}

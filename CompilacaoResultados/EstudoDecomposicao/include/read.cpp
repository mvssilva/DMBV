#include "read.h"

using namespace std;

void setup_ils(Dados * d, char * directory)
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
        process_line_ils(d, line);
    }
}

void process_line_ils(Dados * d, char * line){
    char *file = strrchr(line, '/');
    char *name = strtok(file, ";");
    char * n = strtok(NULL, ";");
    char * m = strtok(NULL, ";");
    char * dbranch = strtok(NULL, ";");
    char *time = strtok(NULL, ";");
    if (time == nullptr) {
        time = (char*) ""; // Define como string vazia para evitar problemas
    }

    if(!dados_exist(d, name))
        dados_add_new(d, name, n, m, dbranch, time);
    else
        dados_add_old(d, name, n, m, dbranch, time);

}

void write_db_mediun(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-mediun.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; Const-ILS; Const-ILSD; R-BEP\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n < 600 && n != naux){

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
            double * vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.1f;", mmedio);

            for(int j = 0; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.2f;", vectordb[j]);
                else
                    fprintf(file, "%.2f", vectordb[j]);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);

}

void write_gap_mediun(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-mediun.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; ILS; ILS-1; ILS-2; ILS-3; ILS-4\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n < 600 && n != naux){

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
            double * vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.1f;", mmedio);

            for(int j = 1; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.2f;", ((vectordb[j] - vectordb[0]) / vectordb[0]) * 100 );
                else
                    fprintf(file, "%.2f", ((vectordb[j] - vectordb[0]) / vectordb[0]) * 100);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);

}

void write_time_mediun(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-mediun.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; Exato; CN7; CN8; R-CN7; R-CN8\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n < 600 && n != naux){

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
            double * vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.1f;", mmedio);

            for(int j = 0; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.2f;", vectortime[j]);
                else
                    fprintf(file, "%.2f", vectortime[j]);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}

void write_db_large(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-large.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; Const-ILS; Const-ILSD; R-BEP\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n >= 600 && m != maux){

            float soma = 0;
            float qtd = 0;
            int indiceArquivo = 0;

            float * vectordb = (float*) malloc(d->qtdArquivos * sizeof(float));
            double *  vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n && d->inst[i].m == m){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.0f;", m);

            for(int j = 0; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.1f;", vectordb[j]);
                else
                    fprintf(file, "%.1f", vectordb[j]);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}


void write_gap_large(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-large.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; ILS; ILS-1; ILS-2; ILS-3; ILS-4\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n >= 600 && m != maux){

            float soma = 0;
            float qtd = 0;
            int indiceArquivo = 0;

            float * vectordb = (float*) malloc(d->qtdArquivos * sizeof(float));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n && d->inst[i].m == m){
                        somadb +=  d->inst[i].db[j];
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.0f;", m);

            for(int j = 1; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.1f;", ((vectordb[j] - vectordb[0]) / vectordb[0]) * 100);
                else
                    fprintf(file, "%.1f", ((vectordb[j] - vectordb[0]) / vectordb[0]) * 100);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}

void write_dbranch_complete(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-complete.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; ils;\n");

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
            double * vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.1f;", mmedio);

            for(int j = 0; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.2f;", vectordb[j]);
                else
                    fprintf(file, "%.2f", vectordb[j]);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}

void write_time_large(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s-large.csv", saida);

    // Abrir o arquivo com o nome concatenado
    FILE *file = fopen(filename, "w");
    fprintf(file, "n'; m'; Exato; CN7; CN8; R-CN7; R-CN8\n");

    float maux = -1;
    float naux = -1;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        float n = inst.n;
        float m = inst.m;
        if(n >= 600 && m != maux){

            float soma = 0;
            float qtd = 0;
            int indiceArquivo = 0;

            float * vectordb = (float*) malloc(d->qtdArquivos * sizeof(float));
            double * vectortime = (double*)malloc(d->qtdArquivos * sizeof(double));

            for(int j = 0; j < d->qtdArquivos; j++){
                float somadb = 0; 
                float somatime = 0;
                qtd = 0;

                for(int i = 0; i < d->sizeInts; i++){
                    if(d->inst[i].n == n && d->inst[i].m == m){
                        somadb +=  d->inst[i].db[j];
                        somatime +=  d->inst[i].time[j];
                        //cout << somadb << " -- " << d->qtdArquivos<< " -- " << d->sizeInts << endl;
                        qtd++;
                    }
                }
                vectordb[j] = somadb / qtd;
                vectortime[j] = somatime / qtd;
            }
            fprintf(file, "%.0f;", n);
            fprintf(file, "%.0f;", m);

            for(int j = 0; j < d->qtdArquivos; j++){
                if(j != d->qtdArquivos - 1)
                    fprintf(file, "%.1f;", vectortime[j]);
                else
                    fprintf(file, "%.1f", vectortime[j]);
            }
            fprintf(file, "\n");
        }
        naux = n;
        maux = m;
    }
    fclose(file);
}

void write_geral(Dados *d, const char * saida)
{
    /*
    Analise total de tempos.
    */

    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s.csv", saida);

    FILE *file = fopen(filename, "w");
    fprintf(file, "Informações; BL-1; BL-2; BL-3; BL-4\n");

    float timeTotal[d->qtdArquivos];

    for(int i = 0; i < d->qtdArquivos; i++)
        timeTotal[i] = 0;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        for(int j = 0; j < d->qtdArquivos; j++){
            timeTotal[j] += inst.time[j];
        }
    }


    fprintf(file, "Tempo total (s);");


    for(int i = 0; i < d->qtdArquivos; i++){
        if(i != d->qtdArquivos - 1)
            fprintf(file, "%0.2f;", timeTotal[i]);
        else
            fprintf(file, "%0.2f\n", timeTotal[i]);
    }


    int minutos[d->qtdArquivos];
    float segundos[d->qtdArquivos];
    for(int i = 0; i < d->qtdArquivos; i++) {
        minutos[i] = (int)timeTotal[i] / 60;
        segundos[i] = timeTotal[i] - (minutos[i] * 60);
    }

    // fprintf(file, "Tempo total (min);");


    // for(int i = 0; i < d->qtdArquivos; i++){
    //     if(i != d->qtdArquivos - 1)
    //         fprintf(file, "%d,%0.f;", minutos[i], segundos[i]);
    //     else
    //         fprintf(file, "%d,%0.f\n", minutos[i], segundos[i]);
    // }

    int melhoresDB[d->qtdArquivos];

    for(int i = 0; i < d->qtdArquivos; i++)
        melhoresDB[i] = 0;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];
        float auxmenor = -1;
        int indicemenor = -1;
        for(int j = 0; j < d->qtdArquivos; j++){
            if(j == 0){
                auxmenor = inst.db[j];
                indicemenor = j;
            }

            if(j != 0 && auxmenor > inst.db[j]){
                auxmenor = inst.db[j];
                indicemenor = j;
            }
            
        }
        
        for(int h = 0; h < d->qtdArquivos; h++){
            float x = inst.db[indicemenor];
            if(inst.db[h] == x)
                melhoresDB[h]++;
        
        }
    }

    fprintf(file, "Best Results;");
    for(int i = 0; i < d->qtdArquivos; i++){
        if(i != d->qtdArquivos - 1)
            fprintf(file, "%d;", melhoresDB[i]);
        else
            fprintf(file, "%d\n", melhoresDB[i]);
    }
    fclose(file);

}

void write_geral_exato(Dados *d, const char * saida)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "output/result-%s.csv", saida);

    FILE *file = fopen(filename, "w");
    fprintf(file, "Informações; ILS; ILS-1; ILS-2; ILS-3; ILS-4\n");
    
    
    /*
    Analise total de tempos.
    */


    float timeTotal[d->qtdArquivos];

    for(int i = 1; i < d->qtdArquivos; i++)
        timeTotal[i] = 0;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];

        for(int j = 1; j < d->qtdArquivos; j++){
            timeTotal[j] += inst.time[j];
        }
    }


    fprintf(file, "Tempo total (s);");


    for(int i = 1; i < d->qtdArquivos; i++){
        if(i != d->qtdArquivos - 1)
            fprintf(file, "%0.2f;", timeTotal[i]);
        else
            fprintf(file, "%0.2f\n", timeTotal[i]);
    }


    // int minutos[d->qtdArquivos];
    // float segundos[d->qtdArquivos];
    // for(int i = 0; i < d->qtdArquivos; i++) {
    //     minutos[i] = (int)timeTotal[i] / 60;
    //     segundos[i] = timeTotal[i] - (minutos[i] * 60);
    // }

    // fprintf(file, "Tempo total (min);");


    // for(int i = 0; i < d->qtdArquivos; i++){
    //     if(i != d->qtdArquivos - 1)
    //         fprintf(file, "%d,%0.f;", minutos[i], segundos[i]);
    //     else
    //         fprintf(file, "%d,%0.f\n", minutos[i], segundos[i]);
    // }


    int iguaisExato[d->qtdArquivos];

    for(int i = 1; i < d->qtdArquivos; i++)
        iguaisExato[i] = 0;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];
        
        for(int h = 1; h < d->qtdArquivos; h++){
            if(inst.db[h] <= inst.db[0])
                iguaisExato[h]++;
        
        }
    }

    fprintf(file, "Comparação com Exatos;");
    for(int i = 1; i < d->qtdArquivos; i++){
        if(i != d->qtdArquivos - 1)
            fprintf(file, "%d;", iguaisExato[i]);
        else
            fprintf(file, "%d\n", iguaisExato[i]);
    }


    int melhoresDB[d->qtdArquivos];

    for(int i = 1; i < d->qtdArquivos; i++)
        melhoresDB[i] = 0;

    for(int i = 0; i < d->sizeInts; i++){
        inst inst = d->inst[i];
        float auxmenor = -1;
        int indicemenor = -1;
        for(int j = 1; j < d->qtdArquivos; j++){
            if(j == 1){
                auxmenor = inst.db[j];
                indicemenor = j;
            }

            if(j != 1 && auxmenor > inst.db[j]){
                auxmenor = inst.db[j];
                indicemenor = j;
            }
        }
        
        for(int h = 1; h < d->qtdArquivos; h++){
            float x = inst.db[indicemenor];
            if(inst.db[h] == x)
                melhoresDB[h]++;
        
        }
    }

    fprintf(file, "Melhores Resultados;");
    for(int i = 1; i < d->qtdArquivos; i++){
        if(i != d->qtdArquivos - 1)
            fprintf(file, "%d;", melhoresDB[i]);
        else
            fprintf(file, "%d\n", melhoresDB[i]);
    }
    fclose(file);

}

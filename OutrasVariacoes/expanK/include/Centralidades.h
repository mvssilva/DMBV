#ifndef CENTRALIDADES_H
#define CENTRALIDADES_H

#include <string>

typedef struct{
    float c;
    int v;
}central_v;

//Máquina UFF
#include </usr/local/include/igraph/igraph.h> 
//Laptop
//#include </opt/local/include/igraph/igraph.h>

void geraOutputFileName(char filename[], int ehcentralidade, int ehsolucao, int option, char subpasta[], char extensao[], char instSet[], char instName[])
{
    char centralidade[25] = "\0";

    strcat(filename, instSet);
   
    if (ehcentralidade){
        switch(option){
            case 0: strcpy(centralidade, "grau");
                    break;
            case 1: strcpy(centralidade, "proximidade");
                    break;
            case 2: strcpy(centralidade, "intermediacao");
                    break;
            case 3: strcpy(centralidade, "pagerank");
                    break;
            case 4: strcpy(centralidade, "autovetor");
                    break;

        }
        strcat(filename, "/");
        strcat(filename, subpasta);
        strcat(filename, "/");
        strcat(filename, centralidade);
        strcat(filename, "/");
        strcat(filename, extensao);
        strcat(filename, "/");
        strcat(filename, centralidade);
        strcat(filename, "_");            
        strcat(filename, instName);
        if(ehsolucao)
            strcat(filename, "_solution_tree");
        strcat(filename, ".");
        strcat(filename, extensao);      
             
    }
    else{
        strcat(filename, "/");
        strcat(filename, subpasta);
        strcat(filename, "/");
        strcat(filename, subpasta);
        strcat(filename, "_");
        strcat(filename, instName);
        strcat(filename, ".");
        strcat(filename, extensao); 
     
    }       
}


void geraMatrizCentralidades(float to[][nc+1], igraph_vector_t* from, int dim, int col)
{
    for(int i=0; i<dim; i++){
        float x = (float) igraph_vector_e(from, i);
        to[i][col] = x;
    }
    
}

void Calcula_Centralidades(igraph_t &g, int n, float matCentralidades[][nc+1], char instSet[], char instName[], int exact, char resultsDir[])
{   
    //Calcula centralidades:
    //**************** Grau **********************
    igraph_vector_t v;
    igraph_vector_t result;
    igraph_vector_init(&result, 0);

    igraph_degree(&g, &result, igraph_vss_all(), IGRAPH_ALL, IGRAPH_LOOPS);

    //cout<<"Calcula centralidade Grau (Degree)"<<endl;
    geraMatrizCentralidades(matCentralidades, &result, n, 0);

   //**************** Closeness **********************
    igraph_vector_init(&result, 0);
    igraph_closeness(&g, &result, NULL, NULL, igraph_vss_all(), IGRAPH_ALL,/*weights=*/ NULL, /*normalized=*/ 0);
    
    //cout<<"Calcula centralidade Proximidade (Closeness)"<<endl;
    geraMatrizCentralidades(matCentralidades, &result, n, 1);

   //**************** Intermediação **********************
    
    igraph_vector_init(&result, 0);
    igraph_betweenness(&g, &result, igraph_vss_all(), IGRAPH_UNDIRECTED, /*weights=*/ NULL);

    //cout<<"Calcula centralidade Intermediacao (Betweenness)"<<endl;
    geraMatrizCentralidades(matCentralidades, &result, n, 2);

    igraph_vector_destroy(&result);
    
    //**************** Pagerank **********************
    
    igraph_vector_t pagerank;
    igraph_real_t value;
    igraph_vector_init(&pagerank, 0);

    igraph_pagerank(&g, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank, &value, igraph_vss_all(), IGRAPH_UNDIRECTED,
                   /* damping */ 0.85, /* weights */ NULL, NULL /* not needed with PRPACK method */);

    //cout<<"Calcula centralidade Pagerank"<<endl;
    geraMatrizCentralidades(matCentralidades, &pagerank, n, 3);

    igraph_vector_destroy(&pagerank);

//**************** autovetor **********************
    
    igraph_vector_t weights;
    igraph_arpack_options_t options;

    igraph_arpack_options_init(&options);
    igraph_vector_init(&v, 0);
    igraph_eigenvector_centrality(&g, &v, &value, /*directed=*/ 0,
                                  /*scale=*/1, /*weights=*/0,
                                  &options);

    //cout<<"Calcula centralidade Autovetor (eigenvector)"<<endl;
    //printf("eigenvalue: %g\n", value);
    //printf("eigenvector:\n");
    //igraph_vector_print(&v);
    geraMatrizCentralidades(matCentralidades, &v, n, 4);

//**************** hub_score **********************

    igraph_arpack_options_init(&options);
    igraph_vector_init(&v, 0);
    igraph_hub_score(&g, &v, &value,
                    /*scale=*/1, /*weights=*/0,
                    &options);

    //cout<<"Calcula centralidade Hub"<<endl;
    //printf("hubsvalue: %g\n", value);
    //printf("hubs:\n");
    //igraph_vector_print(&v);
    geraMatrizCentralidades(matCentralidades, &v, n, 5);
    
//**************** authority_score **********************

    igraph_arpack_options_init(&options);
    igraph_vector_init(&v, 0);
    igraph_authority_score(&g, &v, &value,
                    /*scale=*/1, /*weights=*/0,
                    &options);

    //cout<<"Calcula centralidade Authority"<<endl;
    //printf("authorityvalue: %g\n", value);
    //printf("authority:\n");
    //igraph_vector_print(&v);
    geraMatrizCentralidades(matCentralidades, &v, n, 6);

    igraph_vector_destroy(&v);
    
//**************** Solução **********************    
    //cout<<"Solucao --> calculado em matCentralidades"<<endl;
    if(exact){
        FILE *file = fopen("y_bvector.txt", "r");
        for(int i=0; i< n; i++)
            fscanf(file,"%f", &matCentralidades[i][nc]);        
        fclose(file);
    }else{
        FILE *file = fopen("ILS_Sol_vector.txt", "r");
        for(int i=0; i< n; i++)
            fscanf(file,"%f", &matCentralidades[i][nc]);        
        fclose(file);
    }
    
// Cria subpasta Centralidades com arquivos que contém matriz de centralidades e solução para cada instância
    char filename[200] = {'\0'};
    if(exact){
        strcat(filename, resultsDir);
        strcat(filename, "Exato/");
    }
    else{
        strcat(filename, resultsDir);
        strcat(filename, "Heuristico/");
    }
    
   geraOutputFileName(filename, 0, 0, -1, "Centralidades", "txt", instSet, instName);
    
   FILE *file = fopen(filename, "w+");
    
   for(int i=0; i<n; i++){
       for(int j=0;j<nc+1;j++){
            fprintf(file, "%.6f ", matCentralidades[i][j]); 
       }
       fprintf(file, "\n");
   }
   
   fclose(file);
   
}

void CalculaCorrelacaoPearson(float mC[][nc+1], int n, float CP[][nc])
{
    float media[nc] = {0};
    float num, d1, d2;
    
    for(int j=0;j<nc;j++)
    {
        media[j] = 0;
        for(int i=0; i<n; i++)
            media[j] += mC[i][j];
    }
    for(int j=0;j<nc;j++)
        media[j] /= n;
        

    for(int j=0;j<nc-1;j++){
        for(int k=j+1;k<nc;k++)
        {
            num = d1 = d2 = 0;
            for(int i=0; i<n; i++){
                num += (mC[i][j] - media[j])*(mC[i][k] - media[k]);
                d1 += pow((mC[i][j] - media[j]),2);
                d2 += pow((mC[i][k] - media[k]),2);
            }
            CP[j][k] = num/(sqrt(d1*d2));
        }
        
    }
        
}
int CalculaFScore(float mC[][nc+1], int n, float FS[])
{
    int n_ndb, n_db, i, j;
    float media_ndb, media_db, media, variancia_ndb, variancia_db;

    for(j=0;j<nc;j++)
    {
        n_ndb = n_db = 0;
        media_ndb = media_db = media = 0;
        for(i=0; i<n; i++)
        {           
            if(mC[i][nc] == 0)
            {
                media_ndb += mC[i][j];
                n_ndb++;
            }
            if(mC[i][nc] == 1)
            {
                media_db += mC[i][j];
                n_db++;
            }
            media += mC[i][j];
        }
        if(n_ndb > 0)
            media_ndb /= n_ndb;
        else
            media_ndb = 0;
        if(n_db > 0)
            media_db /= n_db;
        else
            media_db = 0;

        media /= n;

        variancia_ndb = variancia_db = 0;
        for(i=0; i<n; i++)
        {
            if(mC[i][nc] == 0)
                variancia_ndb += pow(mC[i][j] - media_ndb,2);
            if(mC[i][nc] == 1)
                variancia_db += pow(mC[i][j] - media_db,2);
        }
        if(n_ndb > 0)
            variancia_ndb /= n_ndb;
        else
            variancia_ndb = 0;

        if(n_db > 0)
            variancia_db /= n_db;
        else
            variancia_db = 0;

        float numerador = ((float) n_ndb) * pow(media_ndb - media, 2) + ((float) n_db) * pow(media_db - media, 2);
        float denominador = (1/((float) n-2)) * (((float) n_ndb - 1)*variancia_ndb + ((float) n_db - 1)*variancia_db);

        FS[j] = numerador/denominador;
    }
    return(n_db);
}

void GeraSaidaCorrFscore(float matCentralidades[][nc+1], char instSet[], char instName[], int n, int n_db, float corr[][nc], float fscore[], int exact, char resultsDir[])
{
    int sol[n]; 
    for(int i=0;i<n;i++)
        sol[i] = matCentralidades[i][nc]; //pega a coluna da matriz correspondente à solução

    char filename[200] = {'\0'};
    if(exact){
        strcat(filename, resultsDir);
        strcat(filename, "Exato/");
    }
    else{
        strcat(filename, resultsDir);
        strcat(filename, "Heuristico/");
    }

    geraOutputFileName(filename, 0, 0, -1, "Fscore", "txt", instSet, instName); //Define nome do arquivo de saída
    
    FILE *outf;
    outf = fopen(filename, "w"); 
   
//Arquivos gravados na pasta Fscore:

    fputs("\n", outf);

    fputs("\nSolução:\n", outf);  //Imprime a solução
    for(int i=0; i< n; i++)
        fprintf(outf,"%3i ", i);
    fprintf(outf,"\n");
    for(int i=0; i< n; i++)
        fprintf(outf,"%3i ", sol[i]);
    fprintf(outf,"\n");        

    fputs("\nCorrelação de Pearson:\n", outf); //Imprime a matriz de Correlação
    fputs("\n        Grau  Prox  Inter PageR Autov\n", outf);
    fputs("        -----------------------------\n", outf);
    for(int i=0; i< nc; i++){
        switch(i){
            case 0: fprintf(outf,"Grau  | ");
                    break;
            case 1: fprintf(outf,"Prox  | ");
                    break;
            case 2: fprintf(outf,"Inter | ");
                    break;
            case 3: fprintf(outf,"PageR | ");
                    break;
            case 4: fprintf(outf,"Autov | ");
                    break;
            case 5: fprintf(outf,"Hub | ");
                    break;
            case 6: fprintf(outf,"Authority | ");
                    break;

        }
        for(int j=0; j< nc; j++){
            if (i > j)
                fprintf(outf,"%.3f ", corr[j][i]);
            else if (i < j)
                    fprintf(outf,"%.3f ", corr[i][j]);
                 else
                    fprintf(outf,"      ");
        }
        fprintf(outf,"\n");
    }
    fprintf(outf,"\n");

    fputs("\nFscore:\n", outf); //Imprime o Fscore
    
    for(int i=0; i< nc; i++){
        switch(i){
            case 0: fprintf(outf,"Grau  : ");
                    break;
            case 1: fprintf(outf,"Prox  : ");
                    break;
            case 2: fprintf(outf,"Inter : ");
                    break;
            case 3: fprintf(outf,"PageR : ");
                    break;
            case 4: fprintf(outf,"Autov : ");
                    break;
            case 5: fprintf(outf,"Hub   : ");
                    break;
            case 6: fprintf(outf,"Autho : ");
                    break;

        }
        if(n_db)
            fprintf(outf,"%3.3f\n", fscore[i]);
        else
            fprintf(outf,"-1\n");
    }
    fprintf(outf,"\n");

    fclose(outf);

}
void GetName(char set[], char name[], char input[])
{
    int i = 0, j;

    while(input[i] != '/')
        i++;
    j = 0;
    i++;
    while(input[i] != '/'){
        set[j] = input[i];
        i++;
        j++;
    }
    set[j] = '\0';
    i++;
    j=0;
    while(input[i] != '.'){
        name[j] = input[i];
        i++;
        j++;
    }
    name[j] = '\0';
}

//Função para comparar 2 centralidades
bool compareTwoCentralities(central_v a, central_v b)
{    
    if (a.c == b.c)
        return a.v < b.v;

    return(a.c > b.c);
}

int eh_dbranch(int v, int vdbranch[], int dim)
{
    for(int i=0; i<dim; i++){
        if (v == vdbranch[i])
            return 1;
    }
    return 0;
}

float measure0(short int MdMV[][nc], int n_db, int col)
{
    //Measure 0: não utilizei. A medida1 é muito parecida!
    
    float soma=0;
    for(int i=0; i<n_db; i++)
        soma += MdMV[i][col];       
        
    return soma/n_db;
    
}

float measure1(short int MdMV[][nc], int n_db, int col)
{
    //Measure 1 -> k-error
    
    float soma=0;
    for(int i=0; i<n_db; i++){
        if (MdMV[i][col] == 0)
            soma++;
    }        
        
    return soma/n_db;
    
}

void rank_function(central_v cent[], float pos[], int n){
     
    float soma = 0;
    int i=0, k, j;

    for(int v=0; v<n;v++){ //inicializa o vetor de ranking com 1 até n
        pos[v] = v+1;
    }


    while(i<n-1){
        j=i+1;
        k=0;
        pos[i] = j;
        soma = j;
        while (fabs(cent[j].c - cent[i].c) <= 0.00001 && j<n){
            j++;
            soma += j;
            k++;
        }
        if (k>0){
            soma = soma/((float) k+1); 
            for(int l = i; l < i+k+1; l++)
                pos[l]= soma;
        }
        i+= (k+1);
    }
}

float measure2(float pos[], short int MdMV[][nc], int n, int n_db, int col)
{
//Measure 2   

    float somadbranches=0;
    for(int v=0; v<n; v++){
        if (MdMV[v][col])
            somadbranches += pos[v];
    }

    return somadbranches/n_db;    
    
}

int measureCoverage(short int MdMV[][nc], int n, int col)
{
//Measure 3 --> Coverage (não se aplica ao problema)
    int j=n-1;

    while(MdMV[j][col] == 0 && j>=0){
            j--;
    }

    return j+1; 
}

float measure3(short int MdMV[][nc], int n, int col)
{
//Measure 3 --> Ranking Loss
    int conta1=0, conta0 = 0, conta=0;

    for(int i=0; i<n; i++){
        if (MdMV[i][col] == 1){
            conta1++;
            for(int j=0; j<i; j++)
                if(MdMV[j][col] == 0)
                    conta++;
        }
        else
            conta0++;
    }
    float coeficiente = 1.0/(conta1*conta0);
    return coeficiente*conta; 
}

float measure4(float pos[], short int MdMV[][nc], int n, int col)
{
//Measure 4 --> Average Precision

    int conta1=0, conta = 0;
    float soma = 0;
    for(int i=0; i<n; i++){
        if (MdMV[i][col] == 1){
            conta1++;
            conta=0;
            for(int j=0; j<i+1; j++)
                if(MdMV[j][col] == 1)
                    conta++;
            soma += ((float) conta)/pos[i];
        }
    }
    
    float coeficiente = 1.0/conta1;
    return coeficiente*soma; 
}

void ranking(float r1[], float r2[], float r3[], float r4[], short int MdMV[][nc], float matCentralidades[][nc+1], int n_db, int n, int exact)
{
    int dbranchvertices[n_db], j;
    float rank[n];

    //lê vértices branch do arquivo para vetor (sol)

    int sol[n];

    if(exact){
        FILE *file = fopen("y_bvector.txt", "r");
        for(int i=0; i< n; i++)
            fscanf(file,"%i", &sol[i]);        
        fclose(file);
    }else{
        FILE *file = fopen("ILS_Sol_vector.txt", "r");
        for(int i=0; i< n; i++)
            fscanf(file,"%i", &sol[i]);        
        fclose(file);
    }

    j=0;
    for(int i=0; i<n; i++)
        if(sol[i]){
            dbranchvertices[j] = i;
            j++;
        }

    central_v cent[n];

    for(int col=0; col<nc; col++){
        for(int i=0; i<n; i++){        
            cent[i].c = matCentralidades[i][col];
            cent[i].v = i;
        }
        //ordena cent
        sort(cent, cent + n, compareTwoCentralities);
                
        //preenche uma matriz de booleanos relativos aos vértices ordenados pela centralidade

        for(int i=0; i<n; i++){ 
            if(eh_dbranch(cent[i].v, dbranchvertices, n_db))
                MdMV[i][col] = 1;
            else
                MdMV[i][col] = 0;
        }

        //calcula rank considerando empates:
        rank_function(cent, rank, n);

        
        //Calcula medidas de ranking

        r1[col] = measure1(MdMV, n_db, col);
        r2[col] = measure2(rank, MdMV, n, n_db, col); 
        r3[col] = measure3(MdMV, n, col);
        r4[col] = measure4(rank, MdMV, n, col);
    
    }
    
}


#endif // CENTRALIDADES_H

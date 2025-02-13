#define nc 7

#include "src/Graph.h"
#include </opt/ibm/ILOG/CPLEX_Studio2211/cplex/include/ilcplex/ilocplex.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <dirent.h>
#include <cstdlib>
#include <float.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <queue>

#include "src/ReadFile.h"
#include "src/Heuristic.h"
//#include "PlotGrafo.h"
//#include "Centralidades.h"

extern "C"
{
  #include </usr/local/include/igraph/igraph.h> 
}

int NumCut = 0;

int main(int argc, char *argv[])
{
    int min_HV, numb_OH, numb_NH;
    float time_solve, real_time;   
    clock_t start;
    FILE *file;

    if (argc < 8)
    {
        cout << "Faltam argumentos na chamada: dmbv nome_arquivo parametro_d leitor_instancia." << endl;
        cout << "Exemplo: ./dmbv Instancias/small/Spd_RF2_20_27_219.txt 2 2 0 0 0" << endl;
        return 1;
    }

    int d = atoi(argv[2]); // Representa quantos graus o vertice tem que ter para ser um d-brnach. 
    int inst = atoi(argv[3]); // Qual tipo de instancia, variação na funcao de leitura
    int selectionConstruct = atoi(argv[4]);
    int selectionSearch = atoi(argv[5]); // Seleção de visinhos em "firt best" = 0, por "best" = 1;
    int orderingSearch = atoi(argv[6]); // Classificacao por grau = 0, por pagerank = 1;
    int orderingPetu = atoi(argv[7]); // Classificacao por grau = 0, por pagerank = 1;

    /*  
        ILS     = 2 2 0 0 0 0  
        ILS-1   = 2 2 1 0 1 0
        ILS-2   = 2 2 1 0 1 1
        ILS-3   = 2 2 1 1 1 0
        ILS-4   = 2 2 1 1 1 1

        cn1 = 2 2 0 0 0 0
        cn2 = 2 2 0 0 1 0
        cn3 = 2 2 0 1 0 0
        cn4 = 2 2 0 1 1 0
        cn5 = 2 2 0 0 0 1
        cn6 = 2 2 0 0 1 1
        cn7 = 2 2 0 1 0 1
        cn8 = 2 2 0 1 1 1

        r-cn1 = 2 2 1 0 0 0
        r-cn2 = 2 2 1 0 1 0
        r-cn3 = 2 2 1 1 0 0
        r-cn4 = 2 2 1 1 1 0
        r-cn5 = 2 2 1 0 0 1
        r-cn6 = 2 2 1 0 1 1
        r-cn7 = 2 2 1 1 0 1
        r-cn8 = 2 2 1 1 1 1
    */
    
    igraph_set_attribute_table(&igraph_cattribute_table);

    igraph_t g;
    vector<float> pagerank;
    readInstance(inst, argv[1], g, pagerank);    
    time_solve = 0;
    start = clock();

    Heuristic heuristic(d);
    int n = igraph_vcount(&g);
    int m = igraph_ecount(&g);

    bool*OH = new bool[n];
    bool*NH = new bool[n];
    int*L = new int[n];
    bool solved;
    int timeLimit = 3600;

    for(int i = 0; i < n; i++)
    {
        OH[i] = NH[i] = false;
        L[i] = 0;
    }

    srand(igraph_vcount(&g));
    NumCut = 0;
    numb_OH = 0;

    heuristic.ILS(g,L,OH, pagerank, argv[1], selectionConstruct, selectionSearch, orderingSearch, orderingPetu);

    real_time = ((double)clock() - (double)start) / CLOCKS_PER_SEC;
    ofstream RBEPm("output/localsearchOrigin.csv", std::ios_base::app);
    
    RBEPm << argv[1] << ";"<< n << ";" << m << ";" << heuristic.numHV << ";" << endl;
    RBEPm.close();

    igraph_destroy(&g);
    return 0;
}
#include <iostream>
#include </opt/ibm/ILOG/CPLEX_Studio2211/cplex/include/ilcplex/ilocplex.h>
#include <set>
#include <vector>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <float.h>
#include <cstring>
#include <queue>
#include <stdio.h>
#include <dirent.h>
#include <cstdlib>
#include <algorithm>

#include <vector>
#include <random>
#include <iomanip>
 
#define dimMatriz 5

using namespace std;

extern "C"
{
  #include </usr/local/include/igraph/igraph.h> 
}

#include "Grafo.h"
#include "RBEP.h"

void geraMatrizCentralidades(float to[][dimMatriz], igraph_vector_t* from, int dim, int col)
{
    for(int i=0; i<dim; i++){
        float x = (float) igraph_vector_e(from, i);
        to[i][col] = x;
    } 
}

int main(int argc, char * argv[]){

    if (argc < 2){
		cout << "Nome do arquivo não recebido" << endl;
		return 1;
	}

    igraph_set_attribute_table(&igraph_cattribute_table);
    igraph_t g_aux;

    Grafo G = Grafo::LerArquivo(argv[1], g_aux);

    double time_spent = 0.0;
    clock_t start = clock();
    
    Grafo T;
    vector<int> BT;
    vector<int> BTv;
    int BTMin = 1001;
    int BTMax = 0;
    double Time = 0.0;

    string verificado = "True";

    int n = igraph_vcount(&g_aux);

    int seed = 100;
    std::mt19937_64 gen(seed);

    int k = 0;
    
    for(int i=0; i<100; i++)
    {
        double time_spent4 = 0.0;
        clock_t start4 = clock();

        RBEP alg = RBEP(G);

        alg.Oliveira(gen, k);

        clock_t end = clock();
        time_spent += (double)(end - start) / CLOCKS_PER_SEC;

        Time += time_spent;

        T = alg.ObterArvore();
        BT = alg.ObterBranches();
        BTv.push_back(BT.size());

        if(BT.size() < BTMin){
            BTMin = BT.size();
            if(T.ValidarArvore() != true || T.V.size() != G.V.size())
                verificado = "False";
        }

    }

    ofstream RBEPm("expan_k.csv", std::ios_base::app);
    
    float soma = 0.0;
    for(int i : BTv)
       soma = soma + i;
    RBEPm << argv[1] <<  ";"<< G.n << ";" << G.m << ";" << BTMin << ";" << k << ";" << verificado << ";" << endl;
    RBEPm.close();

    
    return 0;
}
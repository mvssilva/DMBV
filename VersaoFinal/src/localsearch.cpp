#include "localsearch.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>

using namespace std;

Graph local_search(Graph& g, Graph& t)
{
    
    Graph t1;
    int n = t.n; 
    
    bool improvement;
    bool * LH;
    int *visitado;

    visitado = new int [n];
    LH = new bool [n];
    
        // Variáveis para o algoritmo de decomposição de grafos, que vai ser estudado posteriormente
        bool*OH = new bool[n];
        bool*NH = new bool[n];
        int*L = new int[n];

        for(int i = 0; i < n; i++)
        {
            OH[i] = NH[i] = false;
            L[i] = 0;
        }

    // INICIO ALGORITMO
    do
    {   
        improvement = Custom_Neighbor(g, t, LH, L, OH, visitado);
        
        if(improvement)
            igraph_copy(&t, &t1);

    }while(improvement);

    return t;
}
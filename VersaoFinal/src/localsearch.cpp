#include "localsearch.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <set>

using namespace std;
#define d 2


Graph local_search(Graph& g, Graph& t)
{
    
    Graph t1;
    int n = t.n; 
    bool improvement;
    
        // Variáveis herdadas do algoritmo de decomposição de grafos
        bool * LH;
        int *visitado;
        bool*OH = new bool[n];
        bool*NH = new bool[n];
        int*L = new int[n];
        
        visitado = new int [n];
        LH = new bool [n];

        for(int i = 0; i < n; i++)
        {
            LH[i] = OH[i] = NH[i] = false;
            L[i] = 0;
        }

    do
    {   
        improvement = custom_Neighbor(g, t, t1, LH, L, OH, visitado);
        
        if(improvement)
            graph_copy(t1, t);

    }while(improvement);

    return t1;
}


void orderer_vertex(Graph &T, vector< pair<float,int> > &S,  int * L, bool *OH)
{
    for(int i = 0; i < T.n; i++)
        {
            if(T.deg[i] + L[i] > d && !OH[i]){
                S.push_back(make_pair(T.pagerank[i], i) );
                min++;
            }
        }

    sort(S.begin(), S.end());
}
bool custom_Neighbor(Graph &G, Graph &T, Graph &T1, bool *LH, int * L, bool *OH, int *visitado)
{
    /*** Retorna a primeira árvore com pelo menos um vértice d-branch a menos ***/
    bool flag;
    int n = G.n;
    int u, v, e_1, e_2;

    /*** Inicialização das variáveis ***/
    graph_copy(T, T1);
    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    
    Graph bestT;
    graph_copy(T, bestT);
    int minAtualDmbv = 0;
    
    orderer_vertex(T, S, n, LH, L, OH, minAtualDmbv);

    int qtdInicialDmbv = minAtualDmbv;
    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        // cout << "dbranch = " << v+1 << endl;
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);
            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);

                if(orderingSearch == 0)
                    FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                else
                    FindEdge_Pagerank(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist, pagerank); 


                // improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);
                
                if(e_1 != -1)   // and also e_2         
                {   
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }

                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
                {   
                    if(selectionSearch == 0)
                        break;
                    
                    int auxDmbv = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (degree(T1, i) + L[i] > d && !OH[i])
                        {
                            auxDmbv++;
                        }
                    }

                    if(auxDmbv < minAtualDmbv){
                        minAtualDmbv = auxDmbv;
                        graph_copy(&bestT, &T1);
                    }
                }
            }

            if(selectionSearch == 0 && (degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d )))
            {
                S.clear();
                return true;
            }
            LH[v] = true;
        }
    }

    graph_copy(&T1, &bestT);
    // graph_destroy(&bestT);
    if((minAtualDmbv  < qtdInicialDmbv) && selectionSearch != 0)
        return true;

    return false;
}
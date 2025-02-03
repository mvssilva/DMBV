#include "localsearch.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <set>
#include <algorithm>

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
        improvement = best_neighbor(g, t, t1, LH, L, OH, visitado);
        
        if(improvement)
            graph_copy(t1, t);

    }while(improvement);

    return t1;
}


void orderer_vertex(Graph &T, vector< pair<float,int> > &S,  bool *LH, int * L, bool *OH, int & currDB)
{
    for(int i = 0; i < T.n; i++)
        {
            if(T.deg[i] + L[i] > d && !OH[i]){
                S.push_back(make_pair(T.pagerank[i], i) );
                currDB++;
            }
        }

    sort(S.begin(), S.end());
}

bool best_neighbor(Graph &G, Graph &T, Graph &T1, bool *LH, int * L, bool *OH, int *visitado)
{
    /*** Retorna a primeira árvore com pelo menos um vértice d-branch a menos ***/
    bool flag;
    int n = G.n;
    int u, v, e_1, e_2;

    /*** Inicialização das variáveis ***/
    /*** T1 = T */
    graph_copy(T, T1);
    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    
    Graph bestT;
    graph_copy(T, bestT);
    int currDB = 0;
    
    orderer_vertex(T, S, LH, L, OH, currDB);

    int startDB = currDB;
    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        // cout << "dbranch = " << v+1 << endl;
        if(T1.deg[v] + L[v] > d)
        {
            vector<int>& adjV = T1.listAdj[v];
            for(int j = 0; j < adjV.size(); j++)
            {
                u = adjV[j];
                flag = T1.deg[u] + L[u] > d ? true : false;
                
                mark_visited(T1, u, v, visitado);

                find_edge(G, T1, LH, L, OH, visitado, u, v, e_1, e_2); 
                
                if(e_1 != -1)   // and also e_2         
                {   
                    remove_edge(T1.listAdj, u,v);
                    add_edge(T1.listAdj, e_1, e_2);
                }

                if(T1.deg[v] + L[v] <= d || (flag && T1.deg[u] + L[u] <= d ))
                {   
                    
                    int auxDB = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (T1.deg[i] + L[i] > d && !OH[i])
                        {
                            auxDB++;
                        }
                    }

                    if(auxDB < currDB){
                        currDB = auxDB;
                        graph_copy(T1, bestT);
                    }
                }
            }
            LH[v] = true;
        }
    }

    graph_copy(bestT, T1);

    if((currDB  < startDB))
        return true;

    return false;
}


void mark_visited(Graph &T1, int u, int v, int *visitado)
{
    list <int> S;
    int x; 
    int n = T1.n;

    for(int i = 0; i < n; i++)
        visitado[i] = -1;

    //avoiding enter in v
    visitado[v] = u;

    visitado[u] = u;
    S.push_front(u);

    while (!S.empty()) {
        x = S.front();
        S.pop_front();
    
        // Acessa diretamente os vizinhos de x
        for (int j = 0; j < T1.listAdj[x].size(); ++j) {
            int aux = T1.listAdj[x][j];  // Vértice vizinho

            if (visitado[aux] != u) {
                visitado[aux] = u;
                S.push_front(aux);
            }
        }
    }

    visitado[u] = v;
    visitado[v] = v;

    S.push_front(v);

    while (!S.empty()) {
        x = S.front();
        S.pop_front();
    
        // Acessa diretamente os vizinhos de x
        for (int j = 0; j < T1.listAdj[x].size(); ++j) {
            int aux = T1.listAdj[x][j];  // Vértice vizinho

            if (visitado[aux] != v) {
                visitado[aux] = v;
                S.push_front(aux);
            }
        }
    }

    //the correct value
    visitado[u] = u;
}

void find_edge(Graph &G, Graph &T1, bool *LH, int *L, bool * OH, int *visitado, int u, int v, int &e_1,int &e_2)
{
    int i, j, optionIJ = CASE_33, optionI = CASE_3, n = G.n;
    int edge[6][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
    float weight[6]= {(float) 2*n, (float) 2*n, (float) 2*n, (float) 2*n, (float) 2*n, (float) 0};
    float wIJ;
    vector<int> adjVert;
    
    e_1 = e_2 = -1;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            bool explore = false;

            if(LH[i])
                {explore = true; optionI = CASE_1;}

            if(optionIJ >= CASE_12 && (T1.deg[i] + L[i] < d || ((i == u || i == v) && T1.deg[i] + L[i] - 1 < d)) )
                {explore = true; optionI = CASE_2;}

            if(!LH[i] && optionIJ >= CASE_13 && (T1.deg[i] + L[i] > d || ((i == u || i == v) && T1.deg[i] + L[i] - 1 > d)) )
                {explore = true; optionI = CASE_3;}

            adjVert = G.listAdj[i];
            for(int k = 0; k < adjVert.size() && explore; ++k)
            {
                j = adjVert[k];
                if(visitado[j] == v)    //the subtree of v
                {
                    if( !((i == u && j == v) || (i == v && j == u)) )
                    {
                        ///select the best candidate edge

                        if(LH[j])       // j = CASE_1
                        {
                            switch(optionI) 
                            {
                              case CASE_1:  optionIJ = CASE_11;
                                            // wIJ = degree(G, i) + degree(G, j) - n*OH[i] - n*OH[j];
                                            wIJ = G.pagerank[i] + G.pagerank[j];
                                            if(wIJ < weight[CASE_11])
                                            {
                                                edge[CASE_11][0] = i;
                                                edge[CASE_11][1] = j;
                                                weight[CASE_11] = wIJ;
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_12)
                                            {
                                                optionIJ = CASE_12;
                                                // wIJ = degree(G, i) - n*OH[i] + degree(G, j);
                                                wIJ = G.pagerank[i] + G.pagerank[j];
                                                if(wIJ < weight[CASE_12])
                                                {
                                                    edge[CASE_12][0] = i;
                                                    edge[CASE_12][1] = j;
                                                    weight[CASE_12] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_13)
                                            {
                                                optionIJ = CASE_13;
                                                // wIJ = degree(G, i) - n*OH[i] - T1.deg[j];
                                                wIJ = G.pagerank[i] + G.pagerank[j];
                                                if(wIJ < weight[CASE_13])
                                                {
                                                    edge[CASE_13][0] = i;
                                                    edge[CASE_13][1] = j;
                                                    weight[CASE_13] = wIJ;
                                                }
                                            }
                                            break;
                            }

                        }

                        if( optionIJ >= CASE_12 && (T1.deg[j]+L[j] < d || ( (j == u || j == v) && T1.deg[j]+L[j]-1 < d)) )  // j = CASE_2
                        {
                            switch(optionI) 
                            {
                              case CASE_1:  if(optionIJ >= CASE_12) //CASE21
                                            {
                                                optionIJ = CASE_12;
                                                wIJ = G.pagerank[i] + G.pagerank[j];
                                                if(wIJ < weight[CASE_12])
                                                {
                                                    edge[CASE_12][0] = i;
                                                    edge[CASE_12][1] = j;
                                                    weight[CASE_12] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_22)
                                            {
                                                optionIJ = CASE_22;
                                                wIJ = G.pagerank[i] + G.pagerank[j];

                                                if(wIJ < weight[CASE_22])
                                                {
                                                    edge[CASE_22][0] = i;
                                                    edge[CASE_22][1] = j;
                                                    weight[CASE_22] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_23)
                                            {
                                                optionIJ = CASE_23;
                                                wIJ = G.pagerank[i] + G.pagerank[j];

                                                if(wIJ < weight[CASE_23])
                                                {
                                                    edge[CASE_23][0] = i;
                                                    edge[CASE_23][1] = j;
                                                    weight[CASE_23] = wIJ;
                                                }
                                            }
                                            break;
                            }
                        }

                        if(!LH[j] && optionIJ >= CASE_13 && (T1.deg[j]+L[j] > d || ((j == u || j == v) && T1.deg[j]+L[j]-1 > d)) )  // j = CASE_3
                        {
                            switch(optionI)
                            {
                              case CASE_1:  if(optionIJ >= CASE_13) //CASE_31
                                            {
                                                optionIJ = CASE_13; 
                                                wIJ = G.pagerank[i] + G.pagerank[j];

                                                if(wIJ < weight[CASE_13])
                                                {
                                                    edge[CASE_13][0] = i;
                                                    edge[CASE_13][1] = j;
                                                    weight[CASE_13] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_23) //CASE_32
                                            {
                                                optionIJ = CASE_23;
                                                wIJ = G.pagerank[i] + G.pagerank[j];

                                                if(wIJ < weight[CASE_23])
                                                {
                                                    edge[CASE_23][0] = i;
                                                    edge[CASE_23][1] = j;
                                                    weight[CASE_23] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_33)
                                            {
                                                optionIJ = CASE_33;
                                                wIJ = G.pagerank[i] + G.pagerank[j];

                                                if(wIJ > weight[CASE_33])
                                                {
                                                    edge[CASE_33][0] = i;
                                                    edge[CASE_33][1] = j;
                                                    weight[CASE_33] = wIJ;
                                                }
                                            }
                                            break;
                            }
                        }

                    }

                }
            }
        }

    }

    e_1 = edge[optionIJ][0];
    e_2 = edge[optionIJ][1];
}

void remove_edge(vector<vector<int>>& listAdj, int src, int dest) {
    // Remove dest da lista de adjacência de src
    listAdj[src].erase(remove(listAdj[src].begin(), listAdj[src].end(), dest), listAdj[src].end());

    // Remove src da lista de adjacência de dest (se o grafo for não direcionado)
    listAdj[dest].erase(remove(listAdj[dest].begin(), listAdj[dest].end(), src), listAdj[dest].end());
}

void add_edge(vector<vector<int>>& listAdj, int src, int dest) {
    if (src == dest)
        return;

    // Adiciona dest à lista de adjacência de src
    listAdj[src].push_back(dest);

    // Adiciona src à lista de adjacência de dest (se for grafo não direcionado)
    listAdj[dest].push_back(src);
}


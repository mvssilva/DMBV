#include <iostream>
#include <fstream>

#include "graph.hpp"
Graph read_graph(const char * file)
{

    int n, m, src, dest, weight;
    char trash[128];

    ifstream f;
    f.open(file);
        
        if(f.fail()){
            cerr << "Erro - Leitura do arquivo de entrada!\n";
            exit(1);
        }
    
    f >> n;
    f >> m;
    f >> trash;
    
    Graph G = Graph(n, m);
    
    for (int i = 0; i < n; i++)
        G.add_vertice(i);
    

    for(int i = 0; i < m; i++)
    {
        f >> src;
        f >> dest;
        f >> weight;
        G.add_edge(src-1,dest-1);
    }

    G.calculate_pagerank();
    
    f.close();
    return G;
}
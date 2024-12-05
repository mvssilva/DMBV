#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <vector>
#include <string>
using namespace std;

class Graph {
    
    protected:
        /*
            Variáveis responsáveis pelo algoritmo 
            de detecção de ramificações/dbranch (DFS)
        */
        int root;
        int t;
        vector<int> PE;
        vector<int> PS;
        vector<int> BACK;
        vector<int> father;
        vector<int> listCT;

        void DFS(int v);
        vector<int> listCC;
        vector<vector<int>> mapCC;

    public:

        string name; 
        int n, m; 
        int *deg; 
        list<int> *adj;


        // Vetores propostos para o Algoritmo Construtivo R-BEP
        vector<int> articulation;
        vector<pair<int,int>> bridges;
        vector<int> V;
        vector<bool> InV;
        vector<pair<int,int>> E;
        vector<vector<int>> listAdj;


        // Construtor padrão/vazio
        Graph() : n(0), m(0), adj(nullptr), deg(nullptr) {}


        // Construtor da leitura, já sabendo a quantidade de vértices e arestas
        Graph(int vertices, int edges) :    n(vertices), m(edges), V(vertices, 0), InV(vertices,false), PE(vertices,0), 
                                            PS(vertices,0), BACK(vertices,0), father(vertices,-1), listCT(vertices,0) {
            adj = new list<int>[n]; 
            deg = new int[n]();

            for (int i = 0; i < n; i++)
            {
                //adj.push_back(vector<int>());
                listCC.push_back(i);
                mapCC.push_back(vector<int>());
                mapCC[i].push_back(i);
            }

            root = 0;
            t = 0;
            listCT[root] = -1;    
        }
        

        ~Graph() {
            delete[] adj;
            delete[] deg;
        }


        void detect_articulations_bridges();
        void updateCC(int v, int u);
        
        bool find_vertice(int v) const;
        void add_vertice(int v);
        void add_edge(int v, int u);
        
        void graph_add_edge(int, int);
        bool graph_exists_edge(int, int) const;
        void graph_print() const;
        
};


#endif // GRAPH_H

#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <vector>
#include <string>
using namespace std;

class Graph {
    
    protected:
        /*
        Variáveis auxiliares para a execução da DFS e detecção de
        pontos de articulação, pontes e componentes conexos.
         */
        int root;                 // Raiz da DFS.
        int t;                    
        vector<int> PE;           
        vector<int> PS;           
        vector<int> BACK;         
        vector<int> father;       // Pai de cada vértice na DFS.
        vector<int> listCT;       

        void DFS(int v);          // Busca em profundidade.

        vector<int> listCC;       // Identificação dos componentes conexos.
        vector<vector<int>> mapCC; // Lista de componentes conexos.

    public:
        
        string name;                  
        int n;                        // Número de vértices.
        int m;                        // Número de arestas.
        vector<vector<int>> listAdj;  // Lista de adjacência para representar o grafo.
        int *deg;                     // Vetor dinâmico que armazena o grau de cada vértice.
        double *pagerank; 
        
        vector<pair<int,int>> edge;   // Lista de todas as arestas (pares de vértices).
        
        vector<int> V;                  // Vetor de vértices do grafo.
        vector<bool> InV;               // Indica se cada vértice está presente no conjunto atual.

        vector<int> articulation;       // Lista de pontos de articulação do grafo.
        vector<pair<int,int>> bridges;  // Lista de arestas que são pontes.

        // Construtor padrão (grafo vazio).
        Graph() : n(0), m(0), deg(nullptr), pagerank(nullptr) {}

        // Construtor que inicializa o grafo com um número fixo de vértices e arestas.
        Graph(int vertices, int edges) :    
            n(vertices), 
            m(edges), 
            V(0, 0), 
            InV(vertices,false), 
            PE(vertices,0), 
            PS(vertices,0), 
            BACK(vertices,0), 
            father(vertices,-1), 
            listCT(vertices,0) 
        {
                deg = new int[n]();
                pagerank  = new double[n]();
                for (int i = 0; i < n; i++)
                {   
                    listAdj.push_back(vector<int>());
                    listCC.push_back(i);
                    mapCC.push_back(vector<int>());
                    mapCC[i].push_back(i);
                }

                root = 0;
                t = 0;
                listCT[root] = -1;    
        }
        

        ~Graph() {
            // delete[] deg;
            // delete[] pagerank;
        }

        friend void graph_copy(Graph& source, Graph& target);

        void update_conection(int v, int u);
        int is_conection(int v) const;
        vector<int> vector_Adjacent(int v) const;
        void detect_articulations_bridges();
        
        bool find_vertice(int v) const;
        void add_vertice(int v);
        void add_edge(int v, int u);

        void calculate_pagerank();        
        int check_connection(int v) const;

        int v_min_pagerank();    
        int u_min_pagerank(int v);    

        int number_dbranch();

        bool graph_exists_edge(int, int) const;
        void graph_print() const;
        
};


#endif // GRAPH_H

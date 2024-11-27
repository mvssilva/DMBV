#ifndef GRAPH_H
#define GRAPH_H

#include <list>

using namespace std;

class Graph {
    public:
        int n, m;
        list<int> *adj;
        int *deg;
        
        Graph(int vertices, int edges) : n(vertices), m(edges) {
            // Inicializando adjacências e graus
            adj = new list<int>[n]; // Aloca lista para os n vértices
            deg = new int[n]();     // Aloca array para graus, inicializado com 0
        }
        
        ~Graph() {
            delete[] adj;
            delete[] deg;
        }

        void graph_add_edge(int, int);
        bool graph_exists_edge(int, int) const;
        void graph_print() const;
        
};


#endif // GRAPH_H

#if !defined(LOCAL_SEARCH__)
#define LOCAL_SEARCH__

#include "graph.hpp"

#define ADJ_SUP 0
#define ADJ_INF 1
#define ADJ_EQUAL 2

#define CASE_1 0
#define CASE_2 1
#define CASE_3 2

#define CASE_11 0
#define CASE_12 1
#define CASE_13 2
#define CASE_22 3
#define CASE_23 4
#define CASE_33 5

Graph local_search(Graph&, Graph&);
bool best_neighbor(Graph &G, Graph &T, Graph &T1, bool *LH, int * L, bool *OH, int *visitado);
void mark_visited(Graph &, int, int, int *);
void find_edge(Graph &G, Graph &T1, bool *LH, int *L, bool * OH, int *visitado, int u, int v, int &e_1,int &e_2);
void remove_edge(Graph&, int src, int dest);
void add_edge(Graph&, int src, int dest);
void print_AdjList(const Graph &T, int vertex);

#endif // LOCAL_SEARCH__

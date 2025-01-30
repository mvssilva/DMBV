#if !defined(LOCAL_SEARCH__)
#define LOCAL_SEARCH__

#include "graph.hpp"


Graph local_search(Graph&, Graph&);
bool custom_Neighbor(Graph &G, Graph &T, Graph &T1, bool *LH, int * L, bool *OH, int *visitado);

#endif // LOCAL_SEARCH__

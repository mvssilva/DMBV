#if !defined(RBEP_H_)
#define RBEP_H_

#include "graph.hpp"
#include <random>

Graph construct_rbep(Graph&);
Graph& alg_rbep(Graph&, std::mt19937_64);
Graph& save_tree(Graph&);

#endif // RBEP_H_

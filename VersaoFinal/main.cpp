
#include <stdio.h>
#include "graph.hpp"
#include "io.hpp"

#define d 2

int main(int argc, char *argv[]){

    Graph G = read_graph(argv[1]);

    G.graph_print();

    return 0;
}
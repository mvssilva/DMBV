
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <set>

#include "io.hpp"
#include "graph.hpp"
#include "construct.hpp"

using namespace std;

#define d 2

int main(int argc, char *argv[]){

    //Leitura do Grafo conexo e não direcionado inicial.
    Graph g = read_graph(argv[1]);

    //Algoritmo Construtivo R-BEP
    Graph t = construct_rbep(g);

    return 0;
}
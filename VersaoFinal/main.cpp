
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set>

#include "io.hpp"
#include "graph.hpp"
#include "construct.hpp"
#include "localsearch.hpp"

using namespace std;

#define d 2

int main(int argc, char *argv[]){

    //Leitura do Grafo conexo e não direcionado inicial.
    Graph g = read_graph(argv[1]);
    
    // Algoritmo Construtivo R-BEP
    Graph t = construct_rbep(g);

    // Busca Local - ILS
    Graph h = local_search(g, t);

    // ofstream result("output/rbep.csv", std::ios_base::app);
    // result << "../" <<argv[1] << ";"<< g.n << ";" << g.m << ";" << t.number_dbranch() << ";" << endl;
    // result.close();
    
    return 0;
}
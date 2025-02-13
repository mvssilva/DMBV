
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

    // t.graph_print();

    // Busca Local - ILS
    Graph h = local_search(g, t);
    
    
    // double n = 0, m = 0;
    // n = g.n;
    // m = g.m;
    // double density = (2 * m) / (n * (n - 1));
    // ofstream result("output/density_alfa25.csv", std::ios_base::app);
    // result << "../" << argv[1] << ";"<< g.n << ";" << g.m << ";"; //<< t.number_dbranch() << ";";// << density << ";";
    // double alfa = 2.5;
    // for (double alfa = 1.5; alfa <= 2.5; alfa += 0.1) {
    //     result << 1 / (alfa * density) << ";";
    // }

    // result << endl;

    // result.close();
    
    return 0;
}
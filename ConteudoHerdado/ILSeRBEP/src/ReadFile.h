#ifndef READFILE_H
#define READFILE_H

#include </usr/local/include/igraph/igraph.h> 
#include <fstream>
#include <filesystem>
#include "Graph.h"

void addpg(float x, vector<float> &pg){
    pg.push_back(x);
}

void readMediumInstances(char *ruta, igraph_t &g, vector<float> &pg)
{
    int n, m, src, dest, weight;
    char trash[128];
  
    ifstream f;
    f.open(ruta);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    f >> n;
    f >> m;
    f >> trash;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    for (int i = 0; i < m; i++)
    {
        f >> src;
        f >> dest;
        f >> weight;

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2
    }

    // std::string path(ruta);
    
    // // Encontrar o nome do arquivo de entrada sem o caminho e a extensão
    // size_t lastSlash = path.find_last_of("/\\"); // Encontra a última barra (para lidar com caminhos no Windows e Unix)
    // size_t lastDot = path.find_last_of("."); // Encontra o último ponto

    // // Extrair o nome do arquivo sem extensão
    // std::string filename = path.substr(lastSlash + 1, lastDot - lastSlash - 1);

    // // Criar o caminho para o arquivo de saída
    // std::string outputFilename = "pagerank/" + filename + ".txt";

    // Abrir o arquivo de saída
    // std::ofstream print(outputFilename);

    igraph_vector_t pagerank;
    igraph_real_t value;
    igraph_vector_init(&pagerank, 0);
    igraph_pagerank(&g, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank, &value, igraph_vss_all(), IGRAPH_UNDIRECTED,

                    /* damping */ 0.85, /* weights */ NULL, NULL /* not needed with PRPACK method */);

    igraph_vector_t* from = &pagerank;
    for (int i = 0; i < n; i++){
        float x = (float) igraph_vector_e(from, i); 
        addpg(x, pg);
        // print << i + 1 << " - " << x << endl;
    }
    igraph_vector_destroy(&pagerank);

    // print.close();
    f.close();
}

void readSteind(char *ruta, igraph_t &g)
{
    int n, m, src, dest, weight;

    ifstream f;
    f.open(ruta);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    f >> n;
    f >> m;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    for (int i = 0; i < m; i++)
    {
        f >> src;
        f >> dest;
        f >> weight;

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2
    }

    f.close();
}

void readLeighton(char *path, igraph_t &g)
{
    int n, m, src, dest;
    char e, trash[128];

    ifstream f;
    f.open(path);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo" << path << endl;
        exit(1);
    }

    for (int i = 0; i < 33; i++)
        f.getline(trash, 128);

    f >> trash;
    f >> trash;
    f >> n;
    f >> m;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    for (int i = 0; i < m; i++)
    {
        f >> e;
        f >> src;
        f >> dest;

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2
    }

    f.close();
}

void readTSPLIB95(char *path, igraph_t &g)
{
    int n, src, dest;
    char trash[128];

    ifstream f;
    f.open(path);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    for (int i = 0; i < 3; i++)
        f.getline(trash, 128);
    f >> trash;
    f >> trash;

    f >> n;

    for (int i = 0; i < 3; i++)
        f.getline(trash, 128);

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    while (true)
    {
        f >> src;
        if (src == -1)
            break;
        f >> dest;

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2
    }

    f.close();
}

void readKlingman(char *path, igraph_t &g)
{
    int n, m, src, dest;
    char e, trash[128];

    ifstream f;
    f.open(path);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    for (int i = 0; i < 22; i++)
        f.getline(trash, 128);

    f >> trash;
    f >> trash;
    f >> n;
    f >> m;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    f >> trash;

    for (int i = 0; i < n; i++)
        f.getline(trash, 128);

    for (int i = 0; i < m; i++)
    {
        f >> e;
        f >> src;
        f >> dest;

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2

        f.getline(trash, 128);
    }

    f.close();
}

void readGraphChen(char *ruta, igraph_t &g)
{
    int n = 0, edge;
    char trash[128];

    ifstream f;
    f.open(ruta);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    f.get(trash, 8);
    f >> n;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    f.getline(trash, 128);
    f.getline(trash, 128);
    f.getline(trash, 128);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            f >> edge;
            if (edge == 0 && i < j) // Adiciona aresta evitando loops, conforme a função addEdge
                addEdge(g, i, j);
        }

        f.getline(trash, 128);
    }

    f.close();
}

void readLikeMerabet(char *ruta, igraph_t &g)/* Like Merabet se assemelha a Steind, mas não possui pesos */
{
    int n, m, src, dest, weight;
    char trash[128];

    ifstream f;
    f.open(ruta);
    if (f.fail())
    {
        cerr << "problema ao abrir o arquivo";
        exit(1);
    }

    /* Lê a primeira linha, que contém n e m: "n m "*/
    f >> n;
    f >> m;

    igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

    /* Lê as próximas linhas, que estão no formato "source destination weight=1" */
    for (int i = 0; i < m; i++)
    {
        f >> src;
        f >> dest;
        f >> weight;/* sempre 1, portanto, não é usado */

        addEdge2(g, src - 1, dest - 1); // Adiciona aresta evitando loops, conforme a função addEdge2
    }

    f.close();
}

void readInstance(int inst, char *path, igraph_t &g, vector<float> &pagerank)
{
    switch (inst)
    {
        case 0:
            readGraphChen(path, g);
            break;
        case 1:
            //readGraphChen(path, g); 
            readLikeMerabet(path, g);
            break;
        case 2:
            readMediumInstances(path, g, pagerank);
            break;
        case 3:
            readMediumInstances(path, g, pagerank);
            break;
        case 4:
            readLeighton(path, g);
            break;
        case 5:
            readSteind(path, g);
            break;
        case 6:
            readTSPLIB95(path, g);
            break;
        case 7:
            readKlingman(path, g);
            break;
    }
}

#endif // READFILE_H

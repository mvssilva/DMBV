#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "graph.hpp"

using namespace std;

void Graph::detect_articulations_bridges()
{
    DFS(root);
}

void graph_copy(Graph &source, Graph &target)
{
    target.name = source.name;
    target.n = source.n;
    target.m = source.m;

    target.edge = source.edge;
    target.listAdj = source.listAdj;
    target.V = source.V;
    target.InV = source.InV;
    target.articulation = source.articulation;
    target.bridges = source.bridges;

    // Liberar memória antiga, se necessário
    if (target.deg) {
        delete[] target.deg;
    }
    if (target.pagerank) {
        delete[] target.pagerank;
    }

    // Copiar os ponteiros
    if (source.deg) {
        target.deg = new int[source.n];
        std::copy(source.deg, source.deg + source.n, target.deg);
    } else {
        target.deg = nullptr;
    }

    if (source.pagerank) {
        target.pagerank = new double[source.n];
        std::copy(source.pagerank, source.pagerank + source.n, target.pagerank);
    } else {
        target.pagerank = nullptr;
    }
}

void Graph::update_conection(int v, int u)
{
    int maxC = max(listCC[v], listCC[u]);
    int minC = min(listCC[v], listCC[u]);

    for(int w : mapCC[maxC])
        listCC[w] = minC;
    mapCC[minC].insert(mapCC[minC].end(), mapCC[maxC].begin(), mapCC[maxC].end());
    mapCC[maxC].clear();

}


int Graph::is_conection(int v) const
{
    return listCC[v];
}


vector<int> Graph::vector_Adjacent(int v) const
{
    return listAdj[v];
}

void Graph::DFS(int v)
{   
    // A função realiza uma busca em profundidade (DFS - Depth-First Search) no grafo.
    // Ela calcula os tempos de entrada (PE) e saída (PS) dos nós na DFS e identifica:
    // - Pontos de articulação (articulation points): Nós cuja remoção desconecta o grafo.
    // - Pontes (bridges): Arestas cuja remoção desconecta o grafo.

    t = t + 1;
	PE[v] = t;
	BACK[v] = t;

	for(int w : listAdj[v])
	{
        if(PE[w] == 0)
        {
            father[w] = v;
			DFS(w);
			if(BACK[w] >= PE[v])
            {
                listCT[v] = listCT[v] + 1;
                if(listCT[v] == 2)
                {
                    articulation.push_back(v);
                }
			}

			if(BACK[v] > BACK[w])
            {
                BACK[v] = BACK[w];
			}

			if(BACK[w] == PE[w])
            {
                bridges.push_back(make_pair(w, v));
			}
        }
        else
        {
            if(PS[w] == 0 && w != father[v])
            {
                if(PE[w] < BACK[v])
                {
                    BACK[v] = PE[w];
                }
            }
        }
	}
	t = t + 1;
	PS[v] = t;
}

bool Graph::find_vertice(int v) const
{
    return (v < n && InV[v] == true);
}

void Graph::add_vertice(int v)
{
    if (find_vertice(v) == false)
    {
        V.push_back(v);
        InV[v] = true;
    }
}

void Graph::add_edge(int src, int dest)
{
    if (src == dest)
        return;
    
    if (graph_exists_edge(src, dest))
       return;

    if (src < dest)
        edge.push_back(make_pair(src,dest));
    else
        edge.push_back(make_pair(dest,src));

    listAdj[src].push_back(dest);
    listAdj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    update_conection(src, dest);
}


void Graph::calculate_pagerank()
{
    const double DAMPING_FACTOR = 0.85; // Fator de amortecimento
    const double EPSILON = 1e-6;       // Critério de convergência

    vector<double> rank(n, 1.0 / n); // Inicializa os ranks uniformemente
    vector<double> newRank(n, 0.0);
    bool converged = false;

    while (!converged) {
        converged = true;

        // Atualiza os ranks para cada nó
        for (int i = 0; i < n; i++) {
            newRank[i] = (1.0 - DAMPING_FACTOR) / n; // Parte de teletransporte
            for (int neighbor : listAdj[i]) {
                newRank[i] += DAMPING_FACTOR * rank[neighbor] / listAdj[neighbor].size();
            }

            // Verifica a convergência
            if (fabs(newRank[i] - rank[i]) > EPSILON) {
                converged = false;
            }
        }

        // Atualiza os ranks
        rank = newRank;
    }

    for (int i = 0; i < n; i++) 
        pagerank[i] = rank[i];
    
}

int Graph::check_connection(int v) const
{
    return listCC[v];
}

int Graph::v_min_pagerank()
{
    float pg = 1;
    int v = -1;

    for (int i = 0; i < n; i++)
    {
        if (pagerank[i] < pg)
        {
            v = i;
            pg = pagerank[i];
        }
    }
    return v;
}

int Graph::u_min_pagerank(int v)
{
    float pg = 1;
    int u = -1;
    for (int i : listAdj[v])
    {
        if (pagerank[i] < pg)
        {
            u = i;
            pg = pagerank[i];
        }
    }
    return u;
}

int Graph::number_dbranch()
{
    
    int qtd = 0;

    for(int i = 0; i < n; i++){
        
        if(deg[i] > 2)
            qtd++;
    }

    return qtd;
}


bool Graph::graph_exists_edge(int src, int dest) const
{
    if (deg[src] > deg[dest])
    {
        int aux = src;
        src = dest;
        dest = aux;
    }

    vector<int>::const_iterator it;

    for (it = listAdj[src].begin(); it != listAdj[src].end(); ++it)
    {
        if (*it == dest)
            return true;
    }
    return false;
}

void Graph::graph_print() const
{
    cout <<"graph G{" << endl;
    for (int i = 0; i < n; i++)
    {
        for (int u : listAdj[i]){
            if(i < u)    
            cout <<"  "<< i+1 << " -- " << u+1 << endl;
        }
    }
    cout << "}" << endl;
}

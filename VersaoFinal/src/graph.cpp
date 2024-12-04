#include <iostream>
#include "graph.hpp"

void Graph::graph_add_edge(int src, int dest)
{
    if (graph_exists_edge(src, dest))
        return;

    if (src == dest)
        return;
    adj[src].push_back(dest);
    adj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    m++;
}

bool Graph::graph_exists_edge(int src, int dest) const
{
    if (deg[src] > deg[dest])
    {
        int aux = src;
        src = dest;
        dest = aux;
    }

    list<int>::iterator it;

    for (it = adj[src].begin(); it != adj[src].end(); ++it)
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
        for (int u : adj[i]){
            if(i < u)    
            cout <<"  "<< i+1 << " -- " << u+1 << endl;
        }
    }
    cout << "}" << endl;
}
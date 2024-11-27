#include "graph.hpp"
#include <iostream>

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
    for (int i = 0; i < n; i++)
    {
        cout << "Degree of vertex " << i << " is : ";
        cout << deg[i] << endl;

        cout << "Adjecency List of vertex " << i << " is : ";
        list<int>::iterator it;
        for (it = adj[i].begin(); it != adj[i].end(); ++it)
            cout << *it << " -> ";
        cout << "\n";
    }
    cout << "Number of vertices: " << n << endl;
    cout << "Number of edges: " << m << endl;
}
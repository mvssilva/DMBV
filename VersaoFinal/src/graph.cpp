#include <iostream>
#include "graph.hpp"


void Graph::detect_articulations_bridges()
{
    DFS(root);
}

void Graph::updateCC(int v, int u)
{
    //cout << "Antes Atualizar: " << nome << " " << v << "(" << listCC[v] << ") - " << u << " (" << listCC[u] << ")" << endl;

    int maxC = max(listCC[v], listCC[u]);
    int minC = min(listCC[v], listCC[u]);

    for(int w : mapCC[maxC])
        listCC[w] = minC;
    mapCC[minC].insert(mapCC[minC].end(), mapCC[maxC].begin(), mapCC[maxC].end());
    mapCC[maxC].clear();

    //cout << "Depois Atualizar: " << v << "(" << listCC[v] << ") - " << u << " (" << listCC[u] << ")" << endl;
}

void Graph::DFS(int root)
{   

    int v = root;
    t = t + 1;
	PE[v] = t;
	BACK[v] = t;

	for(int w : adj[v])
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

vector<int> Graph::list_adj(int v) const
{
    return listAdj[v];
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
    if (graph_exists_edge(src, dest))
        return;

    if (src == dest)
        return;
    
    if (src < dest)
        E.push_back(make_pair(src,dest));
    else
        E.push_back(make_pair(dest,src));

    adj[src].push_back(dest);
    adj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    updateCC(src, dest);
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

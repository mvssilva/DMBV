#include "Graph.h"

Graph::Graph()
{
    n = 0;
    m = 0;
    adj = NULL;
    deg = NULL;
}

Graph::Graph(int numb_vert)
{
    n = numb_vert;
    m = 0;
    adj = new list<int>[n];
    deg = new int[n];
    for (int i = 0; i < n; i++)
        deg[i] = 0;
}

Graph::Graph(const Graph &g)
{
    this->n = g.n;
    this->m = g.m;

    adj = new list<int>[n];
    deg = new int[n];
    for (int i = 0; i < n; i++)
    {
        list<int>::iterator it;
        for (it = g.adj[i].begin(); it != g.adj[i].end(); ++it)
            adj[i].push_back(*it);
        deg[i] = g.deg[i];
    }
}

Graph::~Graph()
{
    delete[] adj;
    delete[] deg;
}

void Graph::clean()
{
    n = m = 0;
    delete[] adj;
    delete[] deg;
}

Graph &Graph::operator=(const Graph &g)
{
    if (this != &g)
    {
        delete[] adj;
        delete[] deg;

        this->n = g.n;
        this->m = g.m;

        adj = new list<int>[n];
        deg = new int[n];
        for (int i = 0; i < n; i++)
        {
            list<int>::iterator it;
            for (it = g.adj[i].begin(); it != g.adj[i].end(); ++it)
                adj[i].push_back(*it);
            deg[i] = g.deg[i];
        }
    }
    return *this;
}

void Graph::set_number_vert(int numb_vert)
{
    n = numb_vert;

    delete[] adj;
    delete[] deg;

    adj = new list<int>[n];
    deg = new int[n];
    for (int i = 0; i < n; i++)
        deg[i] = 0;
}

void Graph::addEdge(int src, int dest)
{
    if (src == dest)
        return;
    adj[src].push_back(dest);
    adj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    m++;
}

void Graph::addEdge2(int src, int dest)
{
    if (existsEdge(src, dest))
        return;

    if (src == dest)
        return;
    adj[src].push_back(dest);
    adj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    m++;
}

void Graph::check_addEdge(int src, int dest)
{

    if (src == dest)
        return;

    if (src > dest)
    {
        int aux = src;
        src = dest;
        dest = aux;
    }

    if (existsEdge(src, dest))
        return;

    adj[src].push_back(dest);
    adj[dest].push_back(src);

    deg[src]++;
    deg[dest]++;

    m++;
}

void Graph::removeEdge(int src, int dest)
{
    adj[src].remove(dest);
    adj[dest].remove(src);

    deg[src]--;
    deg[dest]--;

    m--;
}

bool Graph::existsEdge(int src, int dest) const
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

void Graph::printGraph() const
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

void Graph::Dijkstra(int *distance, int source) const
{
    int n = get_num_vert(), i;
    int *visitado = new int[n];

    for (int i = 0; i < n; i++)
    {
        visitado[i] = 0;
        distance[i] = INF_DIST;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, prioritize> pq; //Priority queue to store vertex,weight pairs

    pq.push(make_pair(source, distance[source] = 0)); //Pushing the source with distance from itself as 0

    while (!pq.empty())
    {
        pair<int, int> curr = pq.top(); //Current vertex. The shortest distance for this has been found
        pq.pop();
        int curr_vert = curr.first, curr_dist = curr.second; //'curr_dist' the final shortest distance for this vertex
        if (visitado[curr_vert])                             //If the vertex is already visited, no point in exploring adjacent vertices
            continue;
        visitado[curr_vert] = true;

        list<int>::iterator it;
        for (it = adj[curr_vert].begin(); it != adj[curr_vert].end(); ++it) //foreach adjacent
        {
            i = *it;
            if (!visitado[i] && (1 + curr_dist < distance[i]))        //If this node is not visited and the current parent node distance+distance from there to this node is shorted than the initial distace set to this node, update it
                pq.push(make_pair(i, (distance[i] = 1 + curr_dist))); //Set the new distance and add to priority queue
        }
    }
}

// Victor: Retorna o número de pontes do grafo
int Graph::detectBridges(bool **UE) const
{
    //Building a arborescence
    int *M = new int[n], *l = new int[n], *prox = new int[n], r, numTag, i, j, k, l_i;
    int num_bridge = m;
    bool **A = new bool *[n];

    for (i = 0; i < n; i++)
    {
        M[i] = 0;
        l[i] = -1;

        A[i] = new bool[n];
        for (j = 0; j < n; j++)
        {
            A[i][j] = false;
            UE[i][j] = false;
        }
    }

    r = rand() % n;
    l[r] = 0;
    prox[r] = -1;
    numTag = 1;

    bool result;

    while (numTag < n)
    {
        l_i = -2;
        for (j = 0; j < n; j++) //could be optimized
            if (l[j] > l_i && M[j] == 0)
            {
                i = j;
                l_i = l[j];
            }

        result = true;
        list<int>::iterator it;
        for (it = adj[i].begin(); it != adj[i].end(); ++it) //foreach adjacent
            if (l[*it] == -1)
            {
                result = false;
                k = *it;
                break;
            }

        if (result)
        {
            M[i] = 1;
            numTag++;
        }
        else
        {
            l[k] = l[i] + 1;
            prox[k] = i;
            A[k][i] = 1;
        }
    }

    //Finding Bridges
    for (int i = 0; i < n; i++)
    {
        list<int>::iterator it;
        for (it = adj[i].begin(); it != adj[i].end(); ++it) //foreach adjacent
        {
            j = *it;
            if (A[i][j] == 0 && A[j][i] != 1 && l[i] > l[j])
            {
                UE[i][j] = UE[j][i] = 1;
                num_bridge--;
                for (k = i; k != j; k = prox[k])
                {
                    UE[k][prox[k]] = UE[prox[k]][k] = 1;
                    num_bridge--;
                }
            }
        }
    }
    delete[] M;
    delete[] prox;
    for (int i = 0; i < n; i++)
        delete[] A[i];
    delete[] A;
    return num_bridge;
}

//Victor: Encontra cociclos que serão utilizados para obter o resultado exato na função Solve_ModelTime
void Graph::findCocycle(list<list<pair<int, int>>> &LCC, int &nCC) const
{
    /**********
        Para determinar los 2-cociclos, se debe eliminar una arista que no sea puente
        (para el momento en que será aplicado no existem puentes) y aplicar el algoritmo para encontrar puentes.
        Todos los puentes, junto con la arista eliminada pertenecen al conjunto de cociclos.
        Guardar la lista en CC. (Ir guardando en la matriz CCdetec se una arista ya fue clasificada dentro de una clase)
        Las aristas serán procesadas se no aparecen como clasificadas en la matriz CCdetec.

        ********/

    int *M = new int[n], *l = new int[n], *prox = new int[n], r, numTag, i, j, k, l_i, u, v;
    bool **A = new bool *[n], **CCdetect = new bool *[n], **UE = new bool *[n];
    list<int>::iterator it;
    list<pair<int, int>> CC;

    nCC = 0;

    for (i = 0; i < n; i++)
    {
        A[i] = new bool[n];
        UE[i] = new bool[n];
        CCdetect[i] = new bool[n];

        for (j = 0; j < n; j++)
            CCdetect[i][j] = false;
    }

    for (u = 0; u < n; u++)
        for (v = u + 1; v < n; v++)
        {
            if (existsEdge(u, v) && !CCdetect[u][v])
            {
                //Building a arborescence

                for (i = 0; i < n; i++)
                {
                    M[i] = 0;
                    l[i] = -1;

                    for (j = 0; j < n; j++)
                    {
                        A[i][j] = false;
                        UE[i][j] = false;
                    }
                }

                r = rand() % n;
                l[r] = 0;
                prox[r] = -1;
                numTag = 1;

                bool result;

                while (numTag < n)
                {
                    l_i = -2;
                    for (j = 0; j < n; j++) //could be optimized
                        if (l[j] > l_i && M[j] == 0)
                        {
                            i = j;
                            l_i = l[j];
                        }

                    result = true;

                    for (it = adj[i].begin(); it != adj[i].end(); ++it)                      //foreach adjacent
                        if (l[*it] == -1 && !((*it == u && i == v) || (*it == v && i == u))) //{u,v} will not considered
                        {
                            result = false;
                            k = *it;
                            break;
                        }

                    if (result)
                    {
                        M[i] = 1;
                        numTag++;
                    }
                    else
                    {
                        l[k] = l[i] + 1;
                        prox[k] = i;
                        A[k][i] = true;
                        //A[i][k] = A[k][i] = 1;
                    }
                }
                //Finding Cocycles

                for (i = 0; i < n; i++)
                {
                    for (it = adj[i].begin(); it != adj[i].end(); ++it) //foreach adjacent
                    {
                        j = *it;
                        if (A[i][j] == false && A[j][i] == false && l[i] > l[j] && !((j == u && i == v) || (j == v && i == u)))
                        {
                            UE[i][j] = UE[j][i] = true;

                            for (k = i; k != j; k = prox[k])
                                UE[k][prox[k]] = UE[prox[k]][k] = true;
                        }
                    }
                }

                //Updating CCdetect
                int num2C = 0;
                for (i = 0; i < n; i++)
                {
                    for (it = adj[i].begin(); it != adj[i].end(); ++it) //foreach adjacent
                    {
                        j = *it;
                        if (UE[i][j] == false && !CCdetect[i][j] && !((*it == u && i == v) || (*it == v && i == u)))
                        {
                            num2C++;
                            CCdetect[i][j] = CCdetect[j][i] = true;
                            CC.push_back(make_pair(i, j));
                        }
                    }
                }
                if (num2C > 0)
                {
                    num2C++;
                    CCdetect[u][v] = CCdetect[v][u] = true;
                    CC.push_back(make_pair(u, v));
                    LCC.push_back(CC);
                    nCC++;
                    CC.clear();
                }
            }
        }

    delete[] M;
    delete[] prox;
    for (int i = 0; i < n; i++)
    {
        delete[] A[i];
        delete[] UE[i];
    }
    delete[] A;
    delete[] UE;
}

void Graph::reset_visited(int *visited, int k) const
{
    for (int i = 0; i < k; i++)
        visited[i] = 0;
}

Graph Graph::G_BFS(int vert, bool **not_bridge, bool *visitado, bool *OH, bool *NH, bool *OH1, bool *NH1, int *L) const
{
    /***crea un grafo a partir de un vértice que no sea obligatoriamente "no branch vertice"**/

    int nH = 0, u, v;
    queue<int> Q;
    map<int, int> mymap;  //map V_G -> V_H
    map<int, int> mymap2; //map V_H -> V_G

    mymap.insert(pair<int, int>(vert, nH));
    mymap2.insert(pair<int, int>(nH, vert));

    v = vert;
    L[mymap[v]] = 0;
    list<int>::iterator it;

    for (it = adj[v].begin(); it != adj[v].end(); it++)
    {
        u = *it;

        if (!not_bridge[u][v]) //it's a bridge
            L[mymap[v]]++;
        else
        {
            Q.push(u);
            visitado[u] = true;
            nH++;
            mymap.insert(pair<int, int>(u, nH)); //hace un mapeo
            mymap2.insert(pair<int, int>(nH, u));
        }
    }

    while (!Q.empty())
    {
        v = Q.front();
        Q.pop();

        L[mymap[v]] = 0;

        for (it = adj[v].begin(); it != adj[v].end(); it++)
        {
            u = *it;

            if (!visitado[u] && u != vert)
            {
                if (!not_bridge[u][v]) //it's a bridge
                    L[mymap[v]]++;
                else
                {
                    Q.push(u);
                    visitado[u] = true;
                    nH++;
                    mymap.insert(pair<int, int>(u, nH));
                    mymap2.insert(pair<int, int>(nH, u));
                }
            }
        }
    }

    //creating the graph H
    Graph H(nH + 1);

    for (int i = 0; i < H.get_num_vert(); i++)
    {
        v = mymap2[i];
        OH1[i] = OH[v];
        NH1[i] = NH[v];
    }

    Q.push(vert);

    while (!Q.empty())
    {
        v = Q.front();
        Q.pop();

        for (it = adj[v].begin(); it != adj[v].end(); it++)
        {
            u = *it;
            if (not_bridge[u][v]) //it's not a bridge
                if (!H.existsEdge(mymap[u], mymap[v]))
                {
                    H.addEdge(mymap[u], mymap[v]);
                    Q.push(u);
                }
        }
    }

    return H;
}

// Victor: Retorna a lista CutH com a quantidade de componentes conexas criadas associadas a cada v em CutH
void Graph::detectCutH(int *Cut_H, list<int> &LCut_H, bool *OH, bool *NH, int *L, int d) const
{
    Graph G2 = *this;
    int *visitados, k;

    visitados = new int[n];

    for (int i = 0; i < n; i++)
        Cut_H[i] = 0;

    for (int v = 0; v < n; v++)
    {
        if (G2.degree(v) + L[v] > d)
        {
            G2.clear_vert(v);

            k = G2.BFS_Conexity(visitados);
            k--;

            if (k + L[v] > d)
            {
                OH[v] = true;
                Cut_H[v] = k; //save the number of connected components
                LCut_H.push_back(v);
            }
            G2.add_all_vert(v, *this);
        }
    }

    delete[] visitados;
}

int Graph::BFS_Conexity(int *visitado) //determina el número de componentes conexas de un grafo
{
    int numComponent, numVisitados, v;
    list<int> Q;
    list<int>::iterator it;

    reset_visited(visitado, n);

    numComponent = 0;
    numVisitados = 0;

    for (int i = 0; (i < n) && (numVisitados < n); i++)
    {
        if (!visitado[i])
        {
            numComponent++;
            visitado[i] = 1;
            numVisitados++;
            Q.push_back(i);

            while (!Q.empty())
            {
                v = Q.front();
                Q.pop_front();

                for (it = adj[v].begin(); it != adj[v].end(); ++it)
                    if (!visitado[*it])
                    {
                        visitado[*it] = 1;
                        numVisitados++;
                        Q.push_back(*it);
                    }
            }
        }
    }
    return numComponent;
}

void Graph::clear_vert(int i)
{
    if (i >= 0 && i < n)
    {
        list<int> Adj = adj[i];
        list<int>::iterator it;
        for (it = Adj.begin(); it != Adj.end(); it++)
            removeEdge(i, *it);
    }
}

void Graph::add_all_vert(int i, const Graph &G)
{
    if (i >= 0 && i < n)
    {
        list<int>::iterator it;
        for (it = G.adj[i].begin(); it != G.adj[i].end(); it++)
            addEdge(i, *it);
    }
}

bool Graph::isCyclicUtil(int v, bool *visited, int parent) const
{
    visited[v] = true;

    list<int>::iterator it;
    for (it = adj[v].begin(); it != adj[v].end(); ++it)
    {
        // If an adjacent is not visited, then recur for that adjacent
        if (!visited[*it])
        {
            if (isCyclicUtil(*it, visited, v))
                return true;
        }
        // If an adjacent is visited and not parent of current vertex,
        // then there is a cycle.
        else if (*it != parent)
            return true;
    }
    return false;
}

// Returns true if the graph contains a cycle, else false.
bool Graph::isCyclic() const
{
    bool *visited = new bool[n], result = false;

    for (int i = 0; i < n; i++)
        visited[i] = false;
    // Call the recursive helper function to detect cycle in different
    // DFS trees
    for (int u = 0; u < n; u++)
        if (!visited[u]) // Don't recur for u if it is already visited
            if (isCyclicUtil(u, visited, -1))
            {
                result = true;
                break;
            }
    delete[] visited;
    return result;
}

int Graph::get_onlyAdj(int i) const
{
    if (!adj[i].empty())
        return *adj[i].begin();
    return -1;
}
/********************* END OF GRAPH CLASS *********************************************/

// ---------------------------- Substituindo as funções da classe Graph para funções que utilizam o igraph ----------------------------
// Retorna a quantidade de pontes do grafo
// Victor: Retorna o número de pontes do grafo
int detectBridges(igraph_t &g, bool **UE)
{
    //Building a arborescence
    int n = igraph_vcount(&g);
    int m = igraph_ecount(&g);
    int *M = new int[n], *l = new int[n], *prox = new int[n], r, numTag, i, j, k, l_i;
    int num_bridge = m;
    bool **A = new bool *[n];

    igraph_adjlist_t adjlist;
    igraph_adjlist_init(&g, &adjlist, IGRAPH_ALL, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVert;

    for (i = 0; i < n; i++)
    {
        M[i] = 0;
        l[i] = -1;

        A[i] = new bool[n];
        for (j = 0; j < n; j++)
        {
            A[i][j] = false;
            UE[i][j] = false;
        }
    }

    r = rand() % n;
    l[r] = 0;
    prox[r] = -1;
    numTag = 1;

    bool result;

    while (numTag < n)
    {
        l_i = -2;
        for (j = 0; j < n; j++) //could be optimized
            if (l[j] > l_i && M[j] == 0)
            {
                i = j;
                l_i = l[j];
            }

        result = true;
        adjVert = igraph_adjlist_get(&adjlist, i);
        for (int w = 0; w < igraph_vector_int_size(adjVert); ++w) //foreach adjacent
        {
            int aux = igraph_vector_int_e(adjVert, w);
            if (l[aux] == -1)
            {
                result = false;
                k = aux;
                break;
            }
        }

        if (result)
        {
            M[i] = 1;
            numTag++;
        }
        else
        {
            l[k] = l[i] + 1;
            prox[k] = i;
            A[k][i] = 1;
        }
    }

    //Finding Bridges
    for (int i = 0; i < n; i++)
    {
        adjVert = igraph_adjlist_get(&adjlist, i);
        for (int w = 0; w < igraph_vector_int_size(adjVert); ++w) //foreach adjacent
        {
            j = igraph_vector_int_e(adjVert, w);
            if (A[i][j] == 0 && A[j][i] != 1 && l[i] > l[j])
            {
                UE[i][j] = UE[j][i] = 1;
                num_bridge--;
                for (k = i; k != j; k = prox[k])
                {
                    UE[k][prox[k]] = UE[prox[k]][k] = 1;
                    num_bridge--;
                }
            }
        }
    }
    delete[] M;
    delete[] prox;
    for (int i = 0; i < n; i++)
        delete[] A[i];
    delete[] A;
    return num_bridge;
}

void removeEdge(igraph_t &g, int src, int dest)
{
    igraph_es_t es;
    igraph_es_pairs_small(&es, IGRAPH_UNDIRECTED, src, dest, -1);
    igraph_delete_edges(&g, es);
    igraph_es_destroy(&es);
}

// Calcula o grau de um vértice
int degree(igraph_t &g, int vertice)
{
    igraph_vector_t v;
    igraph_vector_init(&v, 1);
    igraph_degree(&g, &v, igraph_vss_1(vertice), IGRAPH_ALL, IGRAPH_NO_LOOPS);
    int grau = igraph_vector_e(&v, 0);
    igraph_vector_destroy(&v);
    return grau;
}

// Verifica se a aresta existe
int existsEdge(igraph_t &g, int from, int to)
{
    int ret;
    igraph_integer_t eid;
    igraph_set_error_handler(igraph_error_handler_ignore);
    ret = igraph_get_eid(&g, &eid, from, to, IGRAPH_UNDIRECTED, /*error=*/1);
    if (ret != IGRAPH_EINVAL)
        return 1;

    return 0;
}

void addEdge(igraph_t &g, int src, int dest)
{
    if (src == dest)
        return;
    igraph_add_edge(&g, src, dest);
}

void addEdge2(igraph_t &g, int src, int dest)
{
    if (existsEdge(g, src, dest) || (src == dest))
        return;
    igraph_add_edge(&g, src, dest);
}

igraph_t BFS(igraph_t &g, int vert, bool **not_bridge, bool *visitado, bool *OH, bool *NH, bool *OH1, bool *NH1, int *L)
{
    /***crea un grafo a partir de un vértice que no sea obligatoriamente "no branch vertice"**/
    int nH = 0, u, v;
    queue<int> Q;
    map<int, int> mymap;  //map V_G -> V_H
    map<int, int> mymap2; //map V_H -> V_G

    mymap.insert(pair<int, int>(vert, nH));
    mymap2.insert(pair<int, int>(nH, vert));

    v = vert;
    L[mymap[v]] = 0;
    int i;

    igraph_adjlist_t adjlist;
    igraph_adjlist_init(&g, &adjlist, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVert;
    adjVert = igraph_adjlist_get(&adjlist, v);

    for (i = 0; i < igraph_vector_int_size(adjVert); i++)
    {
        u = igraph_vector_int_e(adjVert, i);

        if (!not_bridge[u][v]) //it's a bridge
            L[mymap[v]]++;
        else
        {
            Q.push(u);
            visitado[u] = true;
            nH++;
            mymap.insert(pair<int, int>(u, nH)); //hace un mapeo
            mymap2.insert(pair<int, int>(nH, u));
        }
    }

    while (!Q.empty())
    {
        v = Q.front();
        Q.pop();
        L[mymap[v]] = 0;
        adjVert = igraph_adjlist_get(&adjlist, v);

        for (i = 0; i < igraph_vector_int_size(adjVert); i++)
        {
            u = igraph_vector_int_e(adjVert, i);

            if (!visitado[u] && u != vert)
            {
                if (!not_bridge[u][v]) //it's a bridge
                    L[mymap[v]]++;
                else
                {
                    Q.push(u);
                    visitado[u] = true;
                    nH++;
                    mymap.insert(pair<int, int>(u, nH));
                    mymap2.insert(pair<int, int>(nH, u));
                }
            }
        }
    }

    //creating the graph H
    igraph_t h;
    igraph_empty(&h, nH + 1, 0);

    for (int i = 0; i < int(igraph_vcount(&h)); i++)
    {
        v = mymap2[i];
        OH1[i] = OH[v];
        NH1[i] = NH[v];
    }

    Q.push(vert);

    while (!Q.empty())
    {
        v = Q.front();
        Q.pop();
        adjVert = igraph_adjlist_get(&adjlist, v);

        for (i = 0; i < igraph_vector_int_size(adjVert); i++)
        {
            u = igraph_vector_int_e(adjVert, i);

            if (not_bridge[u][v]) //it's not a bridge
                if (!existsEdge(h, mymap[u], mymap[v]))
                {
                    addEdge(h, mymap[u], mymap[v]);
                    Q.push(u);
                }
        }
    }

    return h;
}

// Remove todas as arestas de um vértice
void clearVert(igraph_t &g, int i)
{
    igraph_es_t es;
    igraph_es_incident(&es, i, IGRAPH_ALL);
    igraph_delete_edges(&g, es);
}

void reset_visited(int *visited, int k)
{
    for (int i = 0; i < k; i++)
        visited[i] = 0;
}

int BFSConexity(igraph_t &g, int *visitado, int n) //determina el número de componentes conexas de un grafo
{
    int numComponent, numVisitados, v;
    list<int> Q;

    reset_visited(visitado, n);

    numComponent = 0;
    numVisitados = 0;

    igraph_adjlist_t adjlist;
    igraph_adjlist_init(&g, &adjlist, IGRAPH_ALL, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVert;

    for (int i = 0; (i < n) && (numVisitados < n); i++)
    {
        if (!visitado[i])
        {
            numComponent++;
            visitado[i] = 1;
            numVisitados++;
            Q.push_back(i);

            while (!Q.empty())
            {
                v = Q.front();
                Q.pop_front();
                adjVert = igraph_adjlist_get(&adjlist, v);

                for (int j = 0; j < igraph_vector_int_size(adjVert); ++j)
                {
                    int aux = igraph_vector_int_e(adjVert, j);

                    if (!visitado[aux])
                    {
                        visitado[aux] = 1;
                        numVisitados++;
                        Q.push_back(aux);
                    }
                }
            }
        }
    }

    return numComponent;
}

// Adiciona todas as ARESTAS que foram removidas anteriormente
void addAllVert(igraph_t &g2, int v, igraph_t &g, int n, igraph_adjlist_t &adjlist)
{
    igraph_vector_int_t *adjVert;
    adjVert = igraph_adjlist_get(&adjlist, v); 

    if (v >= 0 && v < n)
        for (int i = 0; i < igraph_vector_int_size(adjVert); i++)
            addEdge(g2, v, igraph_vector_int_e(adjVert, i));
}

// Victor: Retorna a lista CutH com a quantidade de componentes conexas criadas associadas a cada v em CutH
void detectCutH(igraph_t &g, int *Cut_H, list<int> &LCut_H, bool *OH, bool *NH, int *L, int d)
{ 
    igraph_t g2 = g; 
    igraph_copy(&g2, &g);
    int *visitados, k;
    int n = igraph_vcount(&g);
    visitados = new int[n];

    igraph_adjlist_t adjlist;
    igraph_adjlist_init(&g, &adjlist, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);

    for (int i = 0; i < n; i++)
        Cut_H[i] = 0;

    for (int v = 0; v < n; v++)
        if (degree(g2, v) + L[v] > d)
        {
            clearVert(g2, v); // Essa função é o problema
            k = BFSConexity(g2, visitados, n);
            k--;

            if (k + L[v] > d)
            {
                OH[v] = true;
                Cut_H[v] = k; //save the number of connected components
                LCut_H.push_back(v);
            }

            addAllVert(g2, v, g, n, adjlist);
        }

    delete[] visitados;
}
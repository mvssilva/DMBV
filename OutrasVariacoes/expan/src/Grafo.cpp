#include "Grafo.h"
#include <iostream>
#include <algorithm>

Grafo::Grafo()
{
    n = 0;
    m = 0;
    t = 0;
}

Grafo::Grafo(int pn, int pm):
    PG(pn,0), InV(pn,false), PE(pn,0), PS(pn,0), BACK(pn,0), pai(pn,-1), ListaCT(pn,0)
{
    n = pn;
    m = pm;

    for (int i = 0; i < n; i++)
    {
        listaAdj.push_back(vector<int>());
        listaCConexa.push_back(i);
        mapaCConexa.push_back(vector<int>());
        mapaCConexa[i].push_back(i);
    }

    raiz = 0;
    t = 0;
    ListaCT[raiz] = -1;
}

Grafo::Grafo(const Grafo& G):
    n(G.n), m(G.m), V(G.V), PG(G.PG), E(G.E), listaAdj(G.listaAdj),
    raiz(G.raiz), PE(G.PE), PS(G.PS), BACK(G.BACK), pai(G.pai), ListaCT(G.ListaCT), listaCConexa(G.listaCConexa), mapaCConexa(G.n,vector<int>())
{
    for (int i = 0; i < n; i++)
        mapaCConexa[i] = vector<int>(G.mapaCConexa[i]);

    t = 0;
}
Grafo Grafo::LerArquivo(string nomeArquivo, igraph_t &g)
{
    std::ifstream arquivoGrafo;
	arquivoGrafo.open(nomeArquivo);
	if (arquivoGrafo.is_open())
    {
        int n,m,lixo;
        arquivoGrafo >> n >> m >> lixo;

        Grafo G(n,m);
        G.nome = nomeArquivo;

        for (int i = 0; i < n; i++){
            G.AdicionarVertice(i);
        }

        igraph_empty(&g, n, 0); // Criando o grafo g com n vértices e 0 arestas

        for (int i = 0; i < m; i++)
        {
            int v, u;
            arquivoGrafo >> v >> u >> lixo;

            v = v - 1;
            u = u - 1;

            G.AdicionarAresta(v,u);
            grafo_add_aresta_igraph(g, v, u);
        }

		arquivoGrafo.close();

        igraph_vector_t pagerank;
        igraph_real_t value;
        igraph_vector_init(&pagerank, 0);
        igraph_pagerank(&g, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank, &value, igraph_vss_all(), IGRAPH_UNDIRECTED,
                        /* damping */ 0.85, /* weights */ NULL, NULL /* not needed with PRPACK method */);

        igraph_vector_t* from = &pagerank;
        for (int i = 0; i < n; i++){
            double x = (double) igraph_vector_e(from, i); 
            G.AdicionarPageRank(x, i);
            // cout << G.PG[i] << endl; 
        }
        igraph_vector_destroy(&pagerank);
		return G;
	}
	else
    {
		throw runtime_error("Erro ao abrir o arquivo");
		return Grafo(0,0);
	}
}

void Grafo::AdicionarPageRank(double pg, int i)
{   
    // cout << pg << endl;
    PG[i] = pg;
}

Grafo::~Grafo()
{
    //dtor
}

void Grafo::DFS(int v)
{
    t = t + 1;
	PE[v] = t;
	BACK[v] = t;

	for(int w : listaAdj[v])
	{
        if(PE[w] == 0)
        {
            pai[w] = v;
			DFS(w);
			if(BACK[w] >= PE[v])
            {
                ListaCT[v] = ListaCT[v] + 1;
                if(ListaCT[v] == 2)
                {
                    ArticulacoesW2.push_back(v);
                }
			}

			if(BACK[v] > BACK[w])
            {
                BACK[v] = BACK[w];
			}

			if(BACK[w] == PE[w])
            {
                Pontes.push_back(make_pair(w, v));
			}
        }
        else
        {
            if(PS[w] == 0 && w != pai[v])
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

void Grafo::DetectarArticulacoesEPontes()
{
    DFS(raiz);
}

void Grafo::AtualizarCConexas(int v, int u)
{
    //cout << "Antes Atualizar: " << nome << " " << v << "(" << listaCConexa[v] << ") - " << u << " (" << listaCConexa[u] << ")" << endl;

    int maxC = max(listaCConexa[v], listaCConexa[u]);
    int minC = min(listaCConexa[v], listaCConexa[u]);

    for(int w : mapaCConexa[maxC])
        listaCConexa[w] = minC;
    mapaCConexa[minC].insert(mapaCConexa[minC].end(), mapaCConexa[maxC].begin(), mapaCConexa[maxC].end());
    mapaCConexa[maxC].clear();

    //cout << "Depois Atualizar: " << v << "(" << listaCConexa[v] << ") - " << u << " (" << listaCConexa[u] << ")" << endl;
}

vector<int> Grafo::Adjacentes(int v) const
{
    return listaAdj[v];
}

int Grafo::Grau(int v) const
{
    return listaAdj[v].size();
}

bool Grafo::BuscarVertice(int v) const
{
    return (v < n && InV[v] == true);
}

void Grafo::AdicionarVertice(int v)
{
    if (BuscarVertice(v) == false)
    {
        V.push_back(v);
        InV[v] = true;
    }
}

// void Grafo::AdicionarPageRank(float pg, int v)
// {   
//     PG.
// }

void Grafo::AdicionarAresta(int v, int u)
{
    if (v < u)
        E.push_back(make_pair(v,u));
    else
        E.push_back(make_pair(u,v));

    listaAdj[v].push_back(u);
    listaAdj[u].push_back(v);
    AtualizarCConexas(v, u);
}

void Grafo::RemoverAresta(int v, int u)
{
    vector<pair<int,int>>::iterator it = E.begin();
    while (it < E.end() && it->first != v && it ->second != u)
        it++;

    if (it < E.end())
        E.erase(it);

    listaAdj[v].erase(std::remove(listaAdj[v].begin(), listaAdj[v].end(), u), listaAdj[v].end());
    listaAdj[u].erase(std::remove(listaAdj[u].begin(), listaAdj[u].end(), v), listaAdj[u].end());
}

int Grafo::CConexa(int v) const
{
    return listaCConexa[v];
}

int Grafo::VerticeGrauMinimo() const
{
    int v = -1, dv = n + 1;
    for (int i = 0; i < n; i++)
    {
        if ((int)listaAdj[i].size() < dv)
        {
            v = i;
            dv = listaAdj[i].size();
        }
    }
    return v;
}


int Grafo::AdjacenteGrauMinimo(int v) const
{
    int u = -1, du = n + 1;
    for (int i : listaAdj[v])
    {
        if ((int)listaAdj[i].size() < du)
        {
            u = i;
            du = listaAdj[i].size();
        }
    }
    return u;
}

int Grafo::NumeroArestas() const
{
    return E.size();
}


bool Grafo::ValidarArvore() const
{
    if (E.size() != V.size() - 1)
        return false;

    for(int i = 0; i < n; i++)
    {
        if (listaCConexa[i] != 0)
        {
            return false;
        }
    }
    return true;
}


void Grafo::ImprimirListaAdj() const
{
    for (int i = 0; i < n; i++)
    {
        cout << i << " - ";
        for (int u : listaAdj[i])
            cout << u << " ";
        cout << endl;
    }
}

void Grafo::ImprimirArvoreGraphviz() const
{   

    // cout <<"graph G{" << endl;
        cout << endl;
    for (int i = 0; i < n; i++)
    {
        for (int u : listaAdj[i]){
            if(i < u)
            cout <<"  "<< i+1 << "--" << u+1 << endl;

        }
    }
    cout << endl;

    // cout << "}" << endl;

}

vector<int> Grafo::RetornaDBranch()
{   
    vector<int> db;
    for(int i = 0; i < n; i++){
        if(Grau(i) > 2)
            db.push_back(i);
        
    }

    return db;
}

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

void grafo_add_aresta_igraph(igraph_t &g, int src, int dest)
{
    if (existsEdge(g, src, dest) || (src == dest))
        return;
    igraph_add_edge(&g, src, dest);
}
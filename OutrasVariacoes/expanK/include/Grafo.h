#if !defined(GRAFO_H_H)
#define GRAFO_H_H

#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include </usr/local/include/igraph/igraph.h> 

using namespace std;

class Grafo
{
    private:
    
    public:
        string nome;

        int n;
        int m;

        vector<bool> InV;
        vector<int> V;
        vector<float> PG;

        vector<pair<int,int>> E;
        vector<vector<int>> listaAdj;

        vector<int> ArticulacoesW2;
        vector<pair<int,int>> Pontes;

        Grafo();
        Grafo(int n, int m = 0);
        Grafo(const Grafo& G);
        static Grafo LerArquivo(string nomeArquivo, igraph_t &g);
        virtual ~Grafo();

        void DetectarArticulacoesEPontes();
        void AtualizarCConexas(int v, int u);

        vector<int> Adjacentes(int v) const;

        int Grau(int v) const;
        bool BuscarVertice(int v) const;

        void AdicionarVertice(int v);
        void AdicionarPageRank(float pg);
        void AdicionarAresta(int v, int u);
        void RemoverAresta(int v, int u);

        int CConexa(int v) const;

        int VerticeGrauMinimo() const;
        int AdjacenteGrauMinimo(int v) const;

        int NumeroArestas() const;

        bool ValidarArvore() const;

        void ImprimirListaAdj() const;
        void ImprimirArvoreGraphviz() const;
    
    protected:
        int raiz;
        int t;
        vector<int> PE;
        vector<int> PS;
        vector<int> BACK;
        vector<int> pai;
        vector<int> ListaCT;

        void DFS(int v);
        vector<int> listaCConexa;
        vector<vector<int>> mapaCConexa;
};

void grafo_add_aresta_igraph(igraph_t &g, int src, int dest);


#endif // GRAFO_H_H

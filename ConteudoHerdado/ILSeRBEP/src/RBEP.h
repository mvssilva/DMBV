#ifndef RBEP_H
#define RBEP_H

#include <set>
#include <vector>
#include <queue>

#include <random>
#include <iomanip>

#include "GrafoRBEP.h"

using namespace std;

class RBEP
{
    protected:
        Grafo G;
        Grafo T;

        vector<int> BT;
        void AtualizarCConexas(int v, int u);

    private:
        vector<bool> InBT;
        vector<bool> InPontas;
        set<int> Pontas;

    public:
        RBEP(const Grafo& pG);
        virtual ~RBEP();

        void Oliveira(std::mt19937_64& gen);

        Grafo ObterArvore() const;
        vector<int> ObterBranches() const;
        vector<int> ObterGrauBT() const;
};

#endif // RBEP_H

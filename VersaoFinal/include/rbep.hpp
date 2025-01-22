#ifndef RBEP_H
#define RBEP_H

#include <set>
#include <vector>
#include <queue>

#include <random>
#include <iomanip>

#include "graph.hpp"

using namespace std;

class RBEP
{
    protected:
        Graph G;
        Graph T;
        vector<int> BT;
        void update_conection(int v, int u);

    private:
        vector<bool> InBT;
        vector<bool> InPontas;
        set<int> Pontas;

    public:
        RBEP(const Graph& pG);
        virtual ~RBEP();

        void apply(std::mt19937_64& gen);

        Graph get_graph() const;
        vector<int> ObterBranches() const;
        vector<int> ObterGrauBT() const;


        int return_size_dbranch();
};

#endif // RBEP_H

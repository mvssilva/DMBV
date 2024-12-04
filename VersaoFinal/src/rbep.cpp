#include "rbep.hpp"
#include <iostream>
#include <vector> 
#include <random>

using namespace std;

Graph& construct_rbep(Graph& g)
{
    Graph t;
    Graph tMin;

    vector<int> dbranch;
    int dbranchMin = 0;

    int seed = 100;
    std::mt19937_64 gen(seed);

    for(int i = 0; i < 100; i++){
        t = alg_rbep(g, gen);

        if(i == 0){
            dbranchMin = dbranch.size();
            tMin = save_tree(t);
        }
        else if(dbranch.size() < dbranchMin){
            dbranchMin = dbranch.size();
            tMin= save_tree(t);
        }
    }

    return tMin;

}

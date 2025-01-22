#include "construct.hpp"
#include "rbep.hpp"
#include <iostream>
#include <vector> 
#include <random>

using namespace std;

Graph construct_rbep(Graph& g)
{
    Graph t;
    Graph tMin;

    int size_dbranch;
    int dbranchMin = 0;

    //Inicialização semente responsavel pela roleta;
    int seed = 100;
    std::mt19937_64 gen(seed);


    for(int i = 0; i < 100; i++){
        
        RBEP rbep = RBEP(g);
        rbep.apply(gen);

        t = rbep.get_graph(); 

        size_dbranch = rbep.return_size_dbranch();
        
        if(i == 0){
            dbranchMin = size_dbranch;
            tMin = t;
        }
        else if(size_dbranch < dbranchMin){
            dbranchMin = size_dbranch;
            tMin = t;
        }
    }

    return tMin; 

}

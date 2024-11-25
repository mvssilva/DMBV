#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Graph.h"
#include "GrafoRBEP.h"
#include <algorithm>

#define ADJ_SUP 0
#define ADJ_INF 1
#define ADJ_EQUAL 2

#define CASE_1 0
#define CASE_2 1
#define CASE_3 2

#define CASE_11 0
#define CASE_12 1
#define CASE_13 2
#define CASE_22 3
#define CASE_23 4
#define CASE_33 5

enum {TIME, ITER, TIME_ITER};

class Heuristic
{
    private:

        //ILS 2
        bool Select_1(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        bool Select_2(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        bool Select_3(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        bool Select_4(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        bool Select_5(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        void Select_6(igraph_t &, int *, int *, int &, int*, bool* , igraph_adjlist_t &);
        void absorve(int* , int , int , int *);

        bool Custom_Neighbor(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &, int, int);

        bool FirstBestNeighbor_by_degree(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);
        bool FirstBestNeighbor_by_pagerank(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);
        
        bool FirstBestNeighborStar_by_degree(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);
        bool FirstBestNeighborStar_by_pagerank(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);
        
        bool BestNeighbor_by_degree(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);
        bool BestNeighbor_by_pagerank(igraph_t &, igraph_t &, bool *, int *, bool *, int *, igraph_adjlist_t &, vector<float> &pagerank, const std::string &, string &);

        void RemoveEdge(igraph_t &, int, int, int *);
        
        void FindEdge_Degree(igraph_t &, igraph_t &, bool *, int *, bool *, int *, int, int, int &,int &, igraph_adjlist_t &);
        void FindEdge_Pagerank(igraph_t &, igraph_t &, bool *, int *, bool *, int *, int, int, int &,int &, igraph_adjlist_t &, vector<float> &pagerank);

        void FindEdgePert_by_degree(igraph_t &, int *, bool *, int *, int, int, int &,int &, bool ** , igraph_adjlist_t &);
        void FindEdgePert_by_pagerank(igraph_t &, int *, bool *, int *, int, int, int &,int &, bool ** , igraph_adjlist_t &, vector<float> &pagerank);
        bool forbMove(int ,int, int, int, bool **, map< pair<int,int> , int > &);
        int d;

    public:
        int time_Limit;
        int num_Iter;
        int typeStop;
        igraph_t T;
        int numHV;
        bool *H;

    //public:
        Heuristic();
        Heuristic(int);
        virtual ~Heuristic();
        void ILS(igraph_t &, int *, bool *,  vector<float> &, const string &, int, int, int, int);
        void InitialConstruction(igraph_t &, int *, bool *);
        void LocalSearch(igraph_t &, int *, bool *, igraph_adjlist_t &, vector<float> &, const std::string &, string &, int, int);
        bool Pertubation_by_degree(igraph_t &, int *, bool *, int *, int &,int &,int &,int &, bool **, igraph_adjlist_t &, vector<float> &pagerank);
        bool Pertubation_by_pagerank(igraph_t &, int *, bool *, int *, int &,int &,int &,int &, bool **, igraph_adjlist_t &, vector<float> &pagerank);
        void make_tree_ils(igraph_t &, int*, bool *, igraph_adjlist_t &);
        void make_tree_rbep(igraph_t &, int*, bool *, igraph_adjlist_t &, const string &, vector<float> &pagerank);

        void setParams(int,int,int,int);
        int get_numHV()const{return numHV;}
        void printH();
        void printH2();
        void getSol(int,bool*,bool**,float*);
        void print_graphviz(std::ofstream&, igraph_t &g);
        void print_graphviz_neighbor(igraph_t &G, igraph_t &T1, int e_1, int e_2, int v, int u, const std::string &path, string &step, int & improvement);

        void saveGraphvizFile(const std::string &path, string &step, igraph_t &g);
    protected:
    private:
};

#endif // HEURISTIC_H

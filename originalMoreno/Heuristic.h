#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Graph.h"
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
        bool Select_1(const Graph &, int *, int *, int &, int*, bool* );
        bool Select_2(const Graph &, int *, int *, int &, int*, bool* );
        bool Select_3(const Graph &, int *, int *, int &, int*, bool* );
        bool Select_4(const Graph &, int *, int *, int &, int*, bool* );
        bool Select_5(const Graph &, int *, int *, int &, int*, bool* );
        void Select_6(const Graph &, int *, int *, int &, int*, bool* );
        void absorve(int* , int , int , int *);


        bool FirstBestN(const Graph &, Graph &, bool *, int *, bool *, int *);
        void RemoveEdge(const Graph &, int, int, int *);
        void FindEdge(const Graph &, const Graph &, bool *, int *, bool *, int *, int, int, int &,int &);
        void FindEdgePert(const Graph &, int *, bool *, int *, int, int, int &,int &, bool **);
        bool forbMove(int ,int, int, int, bool **, map< pair<int,int> , int > &);
        int d;

    public:
        int time_Limit;
        int num_Iter;
        int typeStop;
        Graph T;
        int numHV;
        bool *H;

    //public:
        Heuristic();
        Heuristic(int);
        virtual ~Heuristic();
        void ILS(const Graph &, int *, bool *);
        void InitialConstruction(const Graph&, int *, bool *);
        void LocalSearch(const Graph &, int *, bool *);
        bool Pertubation(const Graph &, int *, bool *, int *, int &,int &,int &,int &, bool **);
        void make_tree(const Graph &, int*, bool *);

        void setParams(int,int,int,int);
        int get_numHV()const{return numHV;}
        void printH();
        void printH2();
        void getSol(int,bool*,bool**,float*);

    protected:
    private:
};

#endif // HEURISTIC_H

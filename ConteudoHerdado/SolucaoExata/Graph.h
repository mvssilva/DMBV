#ifndef GRAPH_H
#define GRAPH_H

    #include <iostream>
    #include <list>
    #include <cstdlib>
    #include <climits>
    #include <queue>
    #include <map>
    #include <vector>

    #define INF_DIST INT_MAX


    using namespace std;

    class Graph
    {
        private:
            bool isCyclicUtil(int , bool *, int)const;

        public:
            int n, m;
            list<int> *adj;
            int *deg;

    	//public:
    	    Graph();
    		Graph(int);
    		Graph(const Graph&);
    		~Graph();

    		Graph & operator= (const Graph &);
    		void set_number_vert(int);
    		void addEdge(int, int);
    		void addEdge2(int, int);
    		void check_addEdge(int, int);
    		void removeEdge(int src, int dest);
    		void clean();

            bool existsEdge(int , int ) const;
            int get_num_vert()const {return n;}
            int get_numb_edges()const {return m;}
            int degree(int i) const {return deg[i];}
            list<int> getList_Adj(int i)const {return adj[i];}
            void printGraph()const;

            void Dijkstra(int*, int) const;
            int detectBridges(bool**)const;
            void reset_visited(int *, int)const;
            Graph G_BFS(int ,bool **, bool *, bool *, bool*, bool *, bool*, int*) const;
            void detectCutH(int *, list <int> &,bool *, bool *, int *, int)const;
            int BFS_Conexity(int *);
            //void deleteBridges(bool **, int);
            void findCocycle(list< list< pair<int,int> > >&, int &)const;
            void clear_vert(int );
            void add_all_vert(int , const Graph&);
            bool isCyclic()const;

            int get_onlyAdj(int i)const;
    };

    class prioritize{
        public:
            bool operator ()(pair<int, int>&p1 ,pair<int, int>&p2) {return p1.second>p2.second;}
    };

#endif // GRAPH_H

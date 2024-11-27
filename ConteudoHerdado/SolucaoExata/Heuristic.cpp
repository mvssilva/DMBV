#include "Heuristic.h"

Heuristic::Heuristic()
{
    time_Limit = 60;
    num_Iter = 100;
    typeStop = 3;
    H = new bool [1];
    numHV = 0;
    d = 2;
}

Heuristic::Heuristic(int dvalue)
{
    time_Limit = 300;
    num_Iter = 100;
    typeStop = 3;
    H = new bool [1];
    numHV = 0;
    d = dvalue;
}


Heuristic::~Heuristic()
{
    if(H)
        delete []H;
}

void Heuristic::setParams(int TL, int NI, int TS, int dvalue)
{
    time_Limit = TL;
    num_Iter = NI;
    typeStop = TS;
    d = dvalue;
}

/***************************ILS*********************************************************************/

void Heuristic::ILS(const Graph &G, int *L, bool *OH)
{
    int n = G.get_num_vert(), u, v, e_1, e_2;

    //InitialConstruction(G,L,OH,d);
    make_tree(G,L,OH);

    if(numHV == 0)
        return;

    LocalSearch(G,L,OH);

    if(numHV == 0)
        return;

    bool **FE = new bool*[n];
    int *visitado = new int[n];

    list<int>::iterator it;
    for(int i = 0; i < n; i++)
    {
        FE[i] = new bool [n];
        for(int j = 0;j < n; j++)
            FE[i][j] = false;
    }

    Graph Tbest = T;
    int numHVbest = numHV;

    clock_t start;
    start = clock();
    float Time;
    //bool finish;
    do
    {
        Time = ((double)clock() - (double)start) / CLOCKS_PER_SEC;
        if(Time > time_Limit-1 )
            break;

        if(!Pertubation(G, L, OH, visitado, u, v, e_1, e_2, FE))
            break;
        else
        {
          FE[e_1][e_2] = FE[e_2][e_1] = true;
        }

        LocalSearch(G,L,OH);

        if(numHV < numHVbest) //improvement
        {
            Tbest = T;
            numHVbest = numHV;
            if(numHVbest == 0)
                break;
            for(int i = 0; i < n; i++)
                for(it = G.adj[i].begin(); it != G.adj[i].end();++it)
                {
                    FE[i][*it] = FE[*it][i]= false;
                }

        }

    }while(true);

    T = Tbest;
    numHV = 0;

    for(int i = 0; i < n; i++)
    {
        if(T.degree(i) + L[i] > d)
        {
            H[i] = true;
            numHV++;
        }
        else
            H[i] = false;
    }

    //printH2();

    for(int i = 0; i < n; i++)
        delete []FE[i];
    delete []FE;

    delete []visitado;

}


/***************************INTIAL CONSTRUCTION*********************************************************************/

void Heuristic::InitialConstruction(const Graph &G, int *L, bool *OH)
{
    int n = G.get_num_vert(), numVisit, maxG, u, v, w;
    bool * S, *AdS;
    int *g = new int[n], *gs = new int[n];
    int dmax[3] = {-1,-1,-1}, vmax[3];

    list<int>::iterator it;

    if(H)
        delete []H;

    S = new bool [n];
    H = new bool [n];
    AdS = new bool [n];

    //Initialize variables

    T.set_number_vert(n);
    numHV = 0;
    numVisit = 0;

    for(u = 0; u < n; u++)
    {
        S[u] = false;
        H[u] = false;
        AdS[u] = false;
        g[u] = G.degree(u) + L[u] + 2*OH[u]*n;
        gs[u] = 0;
    }

    //Find the vertice of max g value
    maxG = -1;

    for(u = 0; u < n; u++)
    {
        if(g[u] > maxG)
        {
            maxG = g[u];
            w = u;
        }
    }

    S[w] = true;
    numVisit++;


    while(numVisit < n)
    {
         //update AdS and
        for(it = G.adj[w].begin(); it != G.adj[w].end(); ++it)
        {
            AdS[*it] = true;
            gs[*it]++;
        }

        //select the vertex to be the point of insertion
        dmax[ADJ_SUP] = dmax[ADJ_INF] = dmax[ADJ_EQUAL] = -1;
        vmax[ADJ_SUP] = vmax[ADJ_INF] = vmax[ADJ_EQUAL] = -1;

        for(u = 0; u < n; u++)
        {
            if(S[u] && gs[u] < G.degree(u)) //T.degree(u)<G.degree(u))
            {
                if(T.degree(u) + L[u] > d || H[u])    //hard vertex
                {
                    if(T.degree(u) > dmax[ADJ_SUP] || (T.degree(u) == dmax[ADJ_SUP] && G.degree(u) > G.degree(vmax[ADJ_SUP])) )
                    {
                        vmax[ADJ_SUP] = u;
                        dmax[ADJ_SUP] = T.degree(u);
                    }
                }
                else
                {
                    if(T.degree(u) + L[u] < d)      // a good vertice
                    {
                        if(g[u] > dmax[ADJ_INF])
                        {
                            vmax[ADJ_INF] = u;
                            dmax[ADJ_INF] = g[u];
                        }
                    }
                    else                            // will be a hard vertice
                    {
                        if(g[u] > dmax[ADJ_EQUAL])
                        {
                            vmax[ADJ_EQUAL] = u;
                            dmax[ADJ_EQUAL] = g[u];
                        }
                    }
                }
            }
        }
        if(vmax[ADJ_SUP] != -1)
            v = vmax[ADJ_SUP];
        else
            if(vmax[ADJ_INF] != -1)
                v = vmax[ADJ_INF];
            else
                v = vmax[ADJ_EQUAL];

        //Select the vert to insert
        maxG = -1;

        for(it = G.adj[v].begin(); it != G.adj[v].end(); ++it)
        {
            u = *it;
            if(!S[u])
            {
                if(g[u]-gs[u] > maxG)
                {
                    maxG = u;
                    w = u;
                }
            }
        }

        //Update H, S and T
        T.addEdge(v,w);
        S[w] = true;
        numVisit++;

        if(!H[v] && (T.degree(v)+L[v] > d || OH[v]))
        {
            H[v] = true;
            numHV++;
        }

        if(!H[w] && (T.degree(w)+L[w] > d || OH[w]))
        {
            H[w] = true;
            numHV++;
        }

    }

}

/***************************LOCAL SEARCH*********************************************************************/

void Heuristic::LocalSearch(const Graph &G, int *L, bool * OH)
{
    Graph T1;
    bool improvement, * LH;
    int n = G.get_num_vert(), *visitado;

    visitado = new int [n];
    LH = new bool [n];

    do
    {
        improvement = FirstBestN(G, T1, LH, L, OH, visitado);
        if(improvement)
            T = T1;
    }while(improvement);

    ///*******Update H and numHV********///

    numHV = 0;
    for(int i = 0; i < n; i++)
    {
        if(T.degree(i) + L[i] > d)
        {
            H[i] = true;
            numHV++;
        }
        else
            H[i] = false;
    }

    delete []visitado;
    delete []LH;
}

bool Heuristic::FirstBestN(const Graph &G, Graph &T1, bool *LH, int * L, bool *OH, int *visitado)
{
    /*** return the first tree with at least one less HV ***/

    bool flag;
    int n = G.get_num_vert(), u, v, e_1, e_2;

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    list<int> AdjV;
    list<int>::iterator it;

    //initialize the variables
    T1 = T;

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(T.degree(i)+L[i] > d && !OH[i])
            S.push_back( make_pair(T.degree(i)+L[i], i) );
    }

    if(S.empty())
        return false;

    sort(S.begin(), S.end());


    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {
        v = (*itS).second;
        if(T1.degree(v)+L[v] > d)
        {
            AdjV = T1.getList_Adj(v);

            for(it = AdjV.begin(); it != AdjV.end(); ++it)
            {
                u = *it;
                flag = T1.degree(u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge(G, T1, LH, L, OH, visitado, u, v, e_1, e_2);

                if(e_1 != -1)   // and also e_2
                {
                    T1.removeEdge(u,v);
                    T1.addEdge(e_1, e_2);
                }
                if(T1.degree(v)+L[v] <= d || (flag && T1.degree(u)+L[u] <= d ))
                    break;
            }

            if(T1.degree(v)+L[v] <= d || (flag && T1.degree(u)+L[u] <= d ))
            {
                S.clear();
                return true;
            }

            LH[v] = true;
        }
    }

    return false;
}

void Heuristic::RemoveEdge(const Graph &T1, int u, int v, int *visitado)
{
    list <int> S;
    list <int> :: iterator it;
    int x, n = T.get_num_vert();

    for(int i = 0; i < n; i++)
        visitado[i] = -1;

    //avoiding enter in v
    visitado[v] = u;

    visitado[u] = u;
    S.push_front(u);

    while(!S.empty())
    {
        x = S.front(); S.pop_front();

        for(it = T1.adj[x].begin(); it != T1.adj[x].end(); ++it)
        {
            if(visitado[*it] != u)
              {
                  visitado[*it] = u;
                  S.push_front(*it);
              }
        }
    }

    //avoiding enter in u
    visitado[u] = v;

    visitado[v] = v;
    S.push_front(v);

    while(!S.empty())
    {
        x = S.front(); S.pop_front();

        for(it = T1.adj[x].begin(); it != T1.adj[x].end(); ++it)
        {
            if(visitado[*it] != v)
              {
                  visitado[*it] = v;
                  S.push_front(*it);
              }
        }
    }
    //the correct value
    visitado[u] = u;
}


void Heuristic::FindEdge(const Graph &G, const Graph &T1, bool *LH, int *L, bool * OH,int *visitado, int u, int v, int &e_1,int &e_2)
{
    int i, j, optionIJ = CASE_33, optionI = CASE_3, n = G.get_num_vert();
    list<int>::iterator it;
    int edge[6][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
    int weight[6]= {2*n, 2*n, 2*n, 2*n, 2*n, 0};
    int wIJ;

    e_1 = e_2 = -1;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            bool explore = false;

            if(LH[i])
                {explore = true; optionI = CASE_1;}

            if(optionIJ >= CASE_12 && (T1.degree(i)+L[i] < d || ((i == u || i == v) && T1.degree(i)+L[i]-1 < d)) )
                {explore = true; optionI = CASE_2;}

            if(!LH[i] && optionIJ >= CASE_13 && (T1.degree(i)+L[i] > d || ((i == u || i == v) && T1.degree(i)+L[i]-1 > d)) )
                {explore = true; optionI = CASE_3;}

            for(it = G.adj[i].begin(); it != G.adj[i].end() && explore; ++it)
            {
                j = *it;
                if(visitado[j] == v)    //the subtree of v
                {
                    if( !((i == u && j == v) || (i == v && j == u)) )
                    {
                        ///select the best candidate edge

                        if(LH[j])       // j = CASE_1
                        {
                            switch(optionI)
                            {
                              case CASE_1:  optionIJ = CASE_11;
                                            wIJ = G.degree(i) + G.degree(j) - n*OH[i] - n*OH[j];
                                            if(wIJ < weight[CASE_11])
                                            {
                                                edge[CASE_11][0] = i;
                                                edge[CASE_11][1] = j;
                                                weight[CASE_11] = wIJ;
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_12)
                                            {
                                                optionIJ = CASE_12;
                                                wIJ = G.degree(i) - n*OH[i] + G.degree(j);
                                                if(wIJ < weight[CASE_12])
                                                {
                                                    edge[CASE_12][0] = i;
                                                    edge[CASE_12][1] = j;
                                                    weight[CASE_12] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_13)
                                            {
                                                optionIJ = CASE_13;
                                                wIJ = G.degree(i) - n*OH[i] - T1.degree(j);
                                                if(wIJ < weight[CASE_13])
                                                {
                                                    edge[CASE_13][0] = i;
                                                    edge[CASE_13][1] = j;
                                                    weight[CASE_13] = wIJ;
                                                }
                                            }
                                            break;
                            }

                        }

                        if( optionIJ >= CASE_12 && ( T1.degree(j)+L[j] < d || ( (j == u || j == v) && T1.degree(j)+L[j]-1 < d)) )  // j = CASE_2
                        {
                            switch(optionI)
                            {
                              case CASE_1:  if(optionIJ >= CASE_12) //CASE21
                                            {
                                                optionIJ = CASE_12;
                                                wIJ = G.degree(i) + G.degree(j) - n*OH[j] ;
                                                if(wIJ < weight[CASE_12])
                                                {
                                                    edge[CASE_12][0] = i;
                                                    edge[CASE_12][1] = j;
                                                    weight[CASE_12] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_22)
                                            {
                                                optionIJ = CASE_22;
                                                wIJ = G.degree(i) + G.degree(j);
                                                if(wIJ < weight[CASE_22])
                                                {
                                                    edge[CASE_22][0] = i;
                                                    edge[CASE_22][1] = j;
                                                    weight[CASE_22] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_23)
                                            {
                                                optionIJ = CASE_23;
                                                wIJ = G.degree(i) - T1.degree(j);
                                                if(wIJ < weight[CASE_23])
                                                {
                                                    edge[CASE_23][0] = i;
                                                    edge[CASE_23][1] = j;
                                                    weight[CASE_23] = wIJ;
                                                }
                                            }
                                            break;
                            }
                        }

                        if(!LH[j] && optionIJ >= CASE_13 && (T1.degree(j)+L[j] > d || ((j == u || j == v) && T1.degree(j)+L[j]-1 > d)) )  // j = CASE_3
                        {
                            switch(optionI)
                            {
                              case CASE_1:  if(optionIJ >= CASE_13) //CASE_31
                                            {
                                                optionIJ = CASE_13;
                                                wIJ = G.degree(j) - n*OH[j] - T1.degree(i) ;
                                                if(wIJ < weight[CASE_13])
                                                {
                                                    edge[CASE_13][0] = i;
                                                    edge[CASE_13][1] = j;
                                                    weight[CASE_13] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_2:  if(optionIJ >= CASE_23) //CASE_32
                                            {
                                                optionIJ = CASE_23;
                                                wIJ = G.degree(j) - T1.degree(i) ;
                                                if(wIJ < weight[CASE_23])
                                                {
                                                    edge[CASE_23][0] = i;
                                                    edge[CASE_23][1] = j;
                                                    weight[CASE_23] = wIJ;
                                                }
                                            }
                                            break;

                              case CASE_3:  if(optionIJ >= CASE_33)
                                            {
                                                optionIJ = CASE_33;
                                                wIJ = T1.degree(j) + T1.degree(i) ;
                                                if(wIJ > weight[CASE_33])
                                                {
                                                    edge[CASE_33][0] = i;
                                                    edge[CASE_33][1] = j;
                                                    weight[CASE_33] = wIJ;
                                                }
                                            }
                                            break;
                            }
                        }

                    }

                }
            }
        }

    }

    e_1 = edge[optionIJ][0];
    e_2 = edge[optionIJ][1];
}

/***************************PERTURBATION*********************************************************************/

bool Heuristic::Pertubation(const Graph &G, int * L, bool *OH, int *visitado, int &u,int &v,int &e_1,int &e_2, bool **FE)
{

    int n = G.get_num_vert();

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    list<int> AdjV;
    list<int>::iterator it;

    //initialize the variables

    for(int i = 0; i < n; i++)
        if(T.degree(i)+L[i] > d && !OH[i])
            S.push_back( make_pair(G.degree(i)+L[i], i) );

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {
        v = (*itS).second;
        AdjV = T.getList_Adj(v);
        for(it = AdjV.begin(); it != AdjV.end(); ++it)
        {
            u = *it;
            RemoveEdge(T, u, v, visitado);
            FindEdgePert(G, L, OH, visitado, u, v, e_1, e_2, FE);

            if(e_1 != -1)   // and also e_2
            {
                T.removeEdge(u,v);
                T.addEdge(e_1, e_2);
                return true;
            }
        }
    }
    u = v = e_1 = e_2 = -1;
    return false;
}


void Heuristic::FindEdgePert(const Graph &G, int *L, bool * OH,int *visitado, int u, int v, int &e_1,int &e_2, bool **FE)
{
    int i, j, n = G.get_num_vert();
    list<int>::iterator it;
    int bestI, bestJ, bestW;

    bestI = bestJ = -1;
    bestW = 2*n;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it)
            {
                j = *it;
                if(visitado[j] == v)    //the subtree of v
                {
                    if( !((i == u && j == v) || (i == v && j == u)) && !FE[i][j] && ((T.deg[i]+L[i] == d && T.deg[j]+L[j] != d) || (T.deg[j]+L[j] == d && T.deg[i]+L[i] != d)) )
                    {
                        ///select the best candidate edge
                        if( G.degree(i)+G.degree(j) < bestW )
                        {
                            bestI = i;
                            bestJ = j;
                            bestW = G.degree(i)+G.degree(j);
                        }

                    }

                }
            }
        }

    }

    e_1 = bestI;
    e_2 = bestJ;
}

bool Heuristic::forbMove(int i,int j, int u, int v, bool **FM, map< pair<int,int> , int > &mapEdge)
{
    pair <int,int> a, b;

    a = (i < j)? make_pair(i,j) : make_pair(j,i);
    b = (u < v)? make_pair(u,v) : make_pair(v,u);

    return FM[mapEdge[a]][mapEdge[b]];
}

/***************************GET_SOL*********************************************************************/

void Heuristic::getSol(int root, bool *y, bool ** x, float *z)
{
    int n = T.get_num_vert(), v, u;

    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            x[i][j] = false;

    for(int i = 0; i < n; i++)
        {
            y[i] = H[i];
            z[i] = 0;
        }

    //exploring with BFS (start vertice = root)
    // Mark all the vertices as not visited
    bool *visited = new bool[n];
    for(int i = 0; i < n; i++)
        visited[i] = false;

    // Create a queue for BFS
    list<int> Q;

    // Mark the current node as visited and enqueue it
    visited[root] = true; z[root] = 0;
    Q.push_back(root);


    list<int>::iterator it;

    int num_visit;

    while(!Q.empty())
    {
        v = Q.front(); Q.pop_front();
        num_visit = 0;

        for(it = T.adj[v].begin(); it != T.adj[v].end(); ++it)
        {
            u = *it;

            if(!visited[u])
            {
                num_visit++;
                visited[u] = true;
                z[u] = z[v] + 1;
                x[v][u] = true;
                Q.push_back(u);
            }
        }

        //if(num_visit == 0 && v != root && d == 1)
        //    z[v] = n-1;
    }
}

void Heuristic::printH2()
{
    for(int i = 0; i< T.get_num_vert(); i++)
        if(H[i])
          {
              cout<<"d("<<i<<") = "<<T.degree(i)<<endl;
          }
    cout<<endl;
}


void Heuristic::printH()
{
    for(int i = 0; i< T.get_num_vert(); i++)
        if(H[i])
            cout<<i<<" ";
    cout<<endl;
}

/******************** INITIAL CONSTRUCTION ********************************/

    void Heuristic::make_tree(const Graph &G, int*L, bool *OH)
    {
        int n = G.get_num_vert();
        int *component = new int[n];
        int *length_component = new int[n], i;

        for(i = 0; i <n; i++)
        {
            component[i] = i;
            length_component[i] = 1;
        }


        int m = 0;
        int selection = 1;
        T.set_number_vert(n);

        if(H)
            delete []H;

        H = new bool [n];

        int option = selection;
        while( m < n-1 )
        {
            switch(selection)
            {
                case 1: if(!Select_1(G, component, length_component, m, L, OH))
                            selection = 3;
                        break;

                case 2: if(Select_2(G, component, length_component, m, L, OH))
                            selection = 3;
                        else
                            selection = 5;
                        break;

                case 3: if(Select_3(G, component, length_component, m, L, OH))
                            selection = 3;
                        else
                            selection = 4;
                        break;

                case 4: if(Select_4(G, component, length_component, m, L, OH))
                            selection = 3;
                        else
                            selection = 2;
                        break;

                case 5: if( Select_5(G, component, length_component, m, L, OH))
                            selection = 3;
                        else
                            selection = 6;
                        break;

                case 6: Select_6(G, component, length_component, m, L, OH);
                        selection = 3;
                        break;
            }

            if(T.get_numb_edges() < m)
            {
                cout<<"Error en selection "<<option<<endl;
            }
            option = selection;

        }

        numHV = 0;

        for(int i = 0; i < n; i++)
        {
            if(T.degree(i) + L[i] > d)
            {
                H[i] = true;
                numHV++;
            }
            else
                H[i] = false;
        }

        delete []component;
        delete []length_component;
    }

    bool Heuristic::Select_1(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int i, min_u = -1, min_v = -1, min_weight = INT_MAX;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i]+L[i] == 0)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it] + L[*it] != d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] - OH[i]*n - OH[*it]*n < min_weight)
                            {
                                min_u = i;
                                min_v = *it;
                                min_weight = G.deg[i] + L[i] + G.deg[*it] + L[*it] - OH[i]*n - OH[*it]*n;
                            }
                    }
            }
            else
            {
                 if(T.deg[i] + L[i] != d)
                 {
                    list<int>::iterator it;
                    for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                        if(T.deg[*it] == 0)
                        {
                            if(component[*it] != component[i])
                                if(G.deg[i]+L[i] + G.deg[*it]+L[*it] - OH[i]*n - OH[*it]*n < min_weight)
                                {
                                    min_u = *it;
                                    min_v = i;
                                    min_weight = G.deg[i]+L[i] + G.deg[*it]+L[*it] - OH[i]*n - OH[*it]*n;
                                }
                        }
                 }
            }
        }

        if(min_u != -1)
        {
            length_component[component[min_v]]++;
            length_component[component[min_u]] = 0;
            component[min_u] = component[min_v];

            T.addEdge(min_u, min_v);
            m++;

            return true;
        }
        return false;
    }


    bool Heuristic::Select_2(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int min_u = -1, min_v = -1, min_weight = INT_MAX, aux, i;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i]+L[i] < d)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it]+L[*it] < d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] - OH[i]*n - OH[*it]*n < min_weight)
                            {
                                min_u = i;
                                min_v = *it;
                                min_weight = G.deg[i] + L[i] + G.deg[*it] + L[*it] - OH[i]*n - OH[*it]*n;
                            }
                    }
            }

        }

        if(min_u != -1)
        {

            if(length_component[component[min_u]] < length_component[component[min_v]])
            {
                aux = min_u;
                min_u = min_v;
                min_v = aux;
            }
            absorve(component, min_u, min_v, length_component);

            T.addEdge(min_u, min_v);
            m++;

            return true;
        }
        return false;
    }


    bool Heuristic::Select_3(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int max_u = -1, max_v = -1, max_weight = 0, aux, i;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i]+L[i] < d)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it]+L[*it] > d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[*it]+L[*it]+OH[*it]*n > max_weight)
                            {
                                max_u = i;
                                max_v = *it;
                                max_weight = G.deg[*it]+L[*it]+OH[*it]*n;
                            }
                    }
            }

        }

        if(max_u != -1)
        {

            if(T.deg[max_u] == 0)
            {
                length_component[component[max_v]]++;
                length_component[component[max_u]] = 0;
                component[max_u] = component[max_v];
            }
            else
            {
                if(length_component[component[max_u]] < length_component[component[max_v]])
                {
                    aux = max_u;
                    max_u = max_v;
                    max_v = aux;
                }
                absorve(component, max_u, max_v, length_component);
            }

            T.addEdge(max_u, max_v);
            m++;

            return true;
        }
        return false;
    }


       bool Heuristic::Select_4(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int max_u = -1, max_v = -1, max_weight = 0, aux, i;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i] + L[i] > d)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it]+L[*it] > d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n > max_weight)
                            {
                                max_u = i;
                                max_v = *it;
                                max_weight = G.deg[i] + L[i] + G.deg[*it] + L[*it]  + OH[i]*n + OH[*it]*n;
                            }
                    }
            }

        }

        if(max_u != -1)
        {

            if(length_component[component[max_u]] < length_component[component[max_v]])
            {
                aux = max_u;
                max_u = max_v;
                max_v = aux;
            }

            absorve(component, max_u, max_v, length_component);

            T.addEdge(max_u, max_v);
            m++;

            return true;
        }
        return false;
    }


    bool Heuristic::Select_5(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int max_u = -1, max_v = -1, max_weight = 0, aux, i;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i] + L[i] > d)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it]+L[*it] == d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n > max_weight)
                            {
                                max_u = i;
                                max_v = *it;
                                max_weight = G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n;
                            }
                    }
            }

        }

        if(max_u != -1)
        {

            if(T.deg[max_u] == 0)
            {
                length_component[component[max_v]]++;
                length_component[component[max_u]] = 0;
                component[max_u] = component[max_v];
            }
            else
            {
                if(length_component[component[max_u]] < length_component[component[max_v]])
                {
                    aux = max_u;
                    max_u = max_v;
                    max_v = aux;
                }
                absorve(component, max_u, max_v, length_component);
            }

            T.addEdge(max_u, max_v);
            m++;

            return true;
        }
        return false;
    }

    void Heuristic::Select_6(const Graph &G, int *component, int *length_component, int &m, int *L, bool *OH)
    {
        int n = G.get_num_vert();
        int max_u1 = -1, max_v1 = -1, max_weight1 = 0, max_u2 = -1, max_v2 = -1, max_weight2 = 0, aux, i;

        for(i = 0; i < n; i++)
        {
            if(T.deg[i]+L[i] == d)
            {
                list<int>::iterator it;
                for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it) //foreach adjacent
                    if(T.deg[*it]+L[*it] < d)
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n > max_weight1)
                            {
                                max_u1 = i;
                                max_v1 = *it;
                                max_weight1 = G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n;
                            }
                    }
                    else    //T.deg[*it] + L[*it] == d
                    {
                        if(component[*it] != component[i])
                            if(G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n > max_weight2)
                            {
                                max_u2 = i;
                                max_v2 = *it;
                                max_weight2 = G.deg[i] + L[i] + G.deg[*it] + L[*it] + OH[i]*n + OH[*it]*n;
                            }
                    }
            }

        }

        if(max_u1 != -1)
        {
            if(length_component[component[max_u1]] < length_component[component[max_v1]])
            {
                aux = max_u1;
                max_u1 = max_v1;
                max_v1 = aux;
            }

            absorve(component, max_u1, max_v1, length_component);

            T.addEdge(max_u1, max_v1);
        }
        else
        {
            if(length_component[component[max_u2]] < length_component[component[max_v2]])
            {
                aux = max_u2;
                max_u2 = max_v2;
                max_v2 = aux;
            }

            absorve(component, max_u2, max_v2, length_component);

            T.addEdge(max_u2, max_v2);
        }

        m++;

    }



  void Heuristic::absorve(int *component, int min_u, int min_v, int *length_component)
  {
        int n = T.get_num_vert();
        length_component[component[min_u]] += length_component[component[min_v]];
        int comp_min_v = component[min_v];

        for(int i = 0; i < n; i++)
            if(component[i] == comp_min_v)
                {
                    component[i] = component[min_u];
                    length_component[i] = 0;
                }
  }


/********** END OF MAKING A TREE ****************/

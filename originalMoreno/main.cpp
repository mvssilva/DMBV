    #include "Graph.h"
    #include </opt/ibm/ILOG/CPLEX_Studio2211/cplex/include/ilcplex/ilocplex.h>
    #include <stdio.h>
    #include <time.h>
    #include <vector>
    #include <dirent.h>
    #include "ReadFile.h"
    #include <queue>
    #include "Heuristic.h"
    #include <cstdlib>


    ILOSTLBEGIN

/******************CALLBACKS*****************************/
int NumCut = 0;

ILOUSERCUTCALLBACK7(CtCallback, IloBoolVarArray, y, IloArray<IloBoolVarArray>, x,
                    IloIntArray, dG, IloIntArray, L, IloBoolArray, OH, IloInt, d, IloInt, root)
{
    if(getNnodes() != 0)
        return;
    //cout<<"In root ;) "<<endl;

    IloInt n = dG.getSize();
    IloEnv env = getEnv();

    for (IloInt i = 0; i < n; i++)
    {
        IloNum y_i = getValue(y[i]);

        if(dG[i]+L[i] > d && L[i] < d && y_i >= 1e-2 ) //&& y_i == IloAbs(y_i) )
        {
            if(i != root)
            {
                //cout<<"NumVar to cut:"<< NumVarToCut++<<endl;

                vector< pair<float, int> > X(n);

                for (IloInt j = 0; j < dG[i]; j++)
                    X.push_back( make_pair(getValue(x[i][j]),j) );
                sort(X.begin(), X.end(), std::greater< pair<float,int> >());

                IloExpr LeftB(env);
                IloNum LeftSumB = 0;

                pair<float,int> Xij;
                IloInt Kb = 0;

                for (; Kb < d-L[i]-1; Kb++)
                {
                    Xij = X[Kb];
                    LeftB += x[i][Xij.second];
                    LeftSumB += Xij.first;
                }

                for (; Kb < dG[i]-1; Kb++)
                {
                    Xij = X[Kb];
                    LeftB += x[i][Xij.second];
                    LeftSumB += Xij.first;

                    if(LeftSumB - 0.1 > ( (Kb+1) + L[i] + 1 - d )*y_i + (d-L[i]-1) )
                    {
                        add(LeftB - ( (Kb+1) + L[i] + 1 - d )*y[i] <= (d-L[i]-1) ).end();
                        NumCut++;
                        //cout<<"Numero de Corte:"<< ++NumCut<<endl;
                        break;
                    }
                }

                LeftB.end();
            }
        }
    }

    //env.end();
}


/**********************************************/

   void findOH_NH(const Graph& G, bool**not_bridge, bool * OH, bool* NH, int &numb_OH, int &numb_NH, int d)
    {
        int n = G.get_num_vert(), numbridges;
        numb_OH = 0;
        numb_NH = 0;

        for(int i = 0; i < n; i++)
        {
            if(G.degree(i) <= d)
              {
                 NH[i] = true;
                 numb_NH++;
              }
            else
            {
                numbridges = 0;
                for(int j = 0; j < n; j++)
                {
                    if(i != j && G.existsEdge(i,j))
                    {
                        if(not_bridge[i][j] == false)
                        {
                            numbridges++;

                            if(numbridges == d)
                                break; //quitar si se precisa la cantidad de puentes
                        }
                    }

                }
                if(numbridges == d)
                {
                    OH[i] = true;
                    numb_OH++;
                }

            }

        }
    }



int root_function(const Graph &G, bool *OH, bool *NH, int *L, int d)
{

    int n = G.get_num_vert();

    if(n == 1)
        return 0;

    int root = -1, sum, maxVal = -1*1e6;
    list<int>::iterator it;

    for(int i = 0; i < n; i++)
    {
        sum = 0;
        for(it = G.adj[i].begin(); it != G.adj[i].end(); ++it)
        {
            sum += G.degree(*it) * ( 1 + OH[*it] );
        }

        if(maxVal < G.degree(i) - (d+1)*L[i] + n*OH[i] + sum)
        {
            maxVal = G.degree(i) - (d+1)*L[i] + n*OH[i] + sum;
            root = i;
        }
    }

    return root;
}


/****************************************************************************/


int Solve_ModelTime(const Graph& H, float &time_solve, bool *OH, bool *NH, int *L,
                    int d, list < list< pair<int,int> > > &LCC, int nCC, int timeLimit,
                    bool &solved, int root, Heuristic& heuristic, int exact)
{
    int n = H.get_num_vert();
    clock_t start;
    start = clock();

	IloEnv env;
	IloModel model(env);

	IloBoolVarArray y(env, n); // y_i
	IloFloatVarArray z(env, n, 0.0 , (n-1)*1.0); // z_i
	IloArray<IloBoolVarArray> x(env, n); //x_ij

	//Add variables to model
	for(int i = 0; i < n; i++)
	{
		IloBoolVarArray x_ij_arr(env, H.degree(i));
		x[i] = x_ij_arr;
	}


	for (int i = 0; i < n; i++)
	{
		model.add(y[i]);
		model.add(z[i]);
	}

    list<int>::iterator it, it2;
    int j = 0;
	for (int i = 0; i < n; i++)
	{
	    j = 0;
		for (it = H.adj[i].begin(); it != H.adj[i].end(); ++it)
		{
		    model.add(x[i][j]);
		    j++;
		}
	}

	//Create Objetive Function
    IloExpr fo(env);
	for(int i = 0; i < n; i++)
	{
		fo += y[i];
	}
	model.add(IloMinimize(env,fo,"f"));

    IloExpr leftR2 = IloExpr(env);
    IloExpr leftR5 = IloExpr(env);
    IloExpr leftR6 = IloExpr(env);

	IloExprArray leftR1 = IloExprArray(env, n);
	IloExprArray leftR4 = IloExprArray(env, n);

	for(int i = 0; i < n; i++)
	{
        if(OH[i] == true)
            model.add( y[i] == 1); // 19) y_i == 1

        if(NH[i])
            model.add( y[i] == 0);  // 20) y_i == 0


        if(i != root)
	    {
            leftR1[i] = IloExpr(env);
            leftR4[i] = IloExpr(env);
	    }

        j = 0;
        int posR, posI;

        for (it = H.adj[i].begin(); it != H.adj[i].end(); ++it)
        {
            posR = posI = 0;

            for (it2 = H.adj[*it].begin(); it2 != H.adj[*it].end(); ++it2, posI++)
                if(*it2 == i)
                    break;
            for (it2 = H.adj[*it].begin(); it2 != H.adj[*it].end(); ++it2, posR++)
                if(*it2 == root)
                    break;

            if(i == root)
                leftR6 += x[*it][posR];

            if(i < *it && i!=root && *it != root)
                        model.add( x[i][j] + x[*it][posI] <= 1);  // 13) x_ij + x_ji <= 1

            if(i == root)   // j != i
                 leftR5 += x[root][j];

            if(*it != root)
            {
                leftR2 += x[i][j];
                model.add( z[*it] >= z[i] + n*x[i][j] - (n-1) + (n-2)*x[*it][posI] );  // 18) z_j >= z_i +1 + (n-1)*(1-x_ij)
            }

            if(i != root)
            {
                leftR1[i] += x[*it][posI];
                if(*it != root)
                    leftR4[i] += x[i][j];
            }

            j++;
        }

        if(i != root)
        {
            model.add(leftR1[i] == 1);   // 11) sum(x_ji) = 1
            model.add(leftR4[i] <= (H.degree(i) + L[i] - d)*y[i] + (d-1) - L[i]);   // 14) sum(x_ij) <= (d_H(i)-d)*y_i + (d-1)
            model.add(leftR4[i] >= d*y[i] - L[i]);
        }

	}

	model.add(leftR2 == n-1);   // 12) sum(x_ij) = n - 1
    model.add(leftR5 <= (H.degree(root) + L[root] - d)*y[root] + d - L[root]);   // 15) sum(x_ri) <= (d_H(r) - d)y_i + (d-1) + 1
    model.add(leftR5 >= d*y[root] - L[root] + 1);
    model.add(leftR6 == 0);   // 16) sum(x_rj) = 0
    model.add(z[root] == 0); //17

        //ADD 2_Cocycles

    list < list< pair<int,int> > >::iterator itl;

    /**** My variant ***/
    IloExprArray leftRCC = IloExprArray(env, nCC);
    int i = 0, posFirst, posSecond;

    for (itl = LCC.begin(); itl != LCC.end(); ++itl)
    {
        leftRCC[i] = IloExpr(env);
        list< pair<int,int> > LC = *itl;
        list< pair<int,int> >::iterator it;
        pair <int,int> edge;

        for (it = LC.begin(); it != LC.end(); ++it)
        {
            edge = *it;
            posFirst = posSecond = 0;

            for (it2 = H.adj[edge.first].begin(); it2 != H.adj[edge.second].end(); ++it2, posSecond++)
                if(*it2 == edge.second)
                    break;
            for (it2 = H.adj[edge.second].begin(); it2 != H.adj[edge.second].end(); ++it2, posFirst++)
                if(*it2 == edge.first)
                    break;

            leftRCC[i] += x[edge.first][posSecond] + x[edge.second][posFirst];
        }
        IloNum nC = LC.size();
        model.add(leftRCC[i] >= nC-1);
        i++;
    }

	IloCplex solver(model);

    solver.setParam(IloCplex::TreLim, 160000); //16000Mb
    solver.setParam(IloCplex::TiLim, timeLimit);
    solver.setParam(IloCplex::Threads, 1);
    solver.setParam(IloCplex::CutUp, heuristic.numHV + 1);

    ///User's Cuts
    IloIntArray  idG(env), iL(env);
    IloBoolArray iOH(env);

    for(int i = 0; i<n; i++)
    {
        idG.add(H.deg[i]);
        iL.add(L[i]);
        iOH.add(OH[i]);
    }

    solver.use(CtCallback(env, y, x, idG, iL, iOH, d, root));

    /*** omitiendo salida ***/
    solver.setOut(env.getNullStream());

	try
	{
		solver.solve();
	}
	catch(IloException& e)
	{
		cout << e;
	}

    float Time = ((double)clock() - (double)start) / CLOCKS_PER_SEC;

    time_solve = Time;
	float res = 0;

	try
	{
		IloNum objval = solver.getObjValue ();
		res= objval;

		if(solver.getStatus() == IloAlgorithm::Optimal)
            solved = true;
        else
            solved = false;
	}
	catch(IloCplex::Exception &e)
    {
		cout << e;
	}
	env.end();

	return res;
}

int Solve_Model(const Graph& H, float &time_solve, bool *OH, bool *NH, int *L, int d, int exact)
{
    int n = H.get_num_vert();
    clock_t start;

    if(n < 3)
        {time_solve = 0; return 0;}

    start = clock();

    Heuristic heuristic(d);
    heuristic.ILS(H,L,OH);

    /***********/
    if(!exact){
        time_solve = ((double)clock() - (double)start) / CLOCKS_PER_SEC;
        return heuristic.numHV;
    }
    /***********/


    list < list< pair<int,int> > > LCC;
    int nCC = -1;

    H.findCocycle(LCC,nCC);

    bool solved = false;
    int root = root_function(H,OH, NH, L,d), best_result, timeLimit = 3600;

    best_result = Solve_ModelTime(H, time_solve, OH, NH, L, d, LCC, nCC, timeLimit, solved, root,
                                 heuristic, exact);

    time_solve = ((double)clock() - (double)start) / CLOCKS_PER_SEC;

    return best_result;
}



int Solve_Graph(const Graph& G,float &time_solve, bool *OH, bool *NH, int *L, int d, list <int> &LCut_H, int *Cut_H, int exact)
{
    int sol = 0, v, k; //, n = G.get_num_vert();
    float partial_time;
    time_solve = 0;

    if(LCut_H.empty())
        return Solve_Model(G,time_solve, OH, NH,L,d, exact);
    else
    {
        //applied just one cut

        v = LCut_H.front(); LCut_H.pop_front();
        k = Cut_H[v];
        Cut_H[v] = 0;

        if(k == 1)
            return Solve_Graph(G,time_solve, OH, NH, L, d, LCut_H, Cut_H, exact);

        Graph G2 = G;

        int numVisitados, u, n = G2.get_num_vert();
        int *visitado, *visitado2, *L1, *Cut_H1;

        visitado = new int[n];
        visitado2 = new int[n];
        L1 = new int [n];
        Cut_H1 = new int [n];

        bool *OH1 = new bool [n];
        bool *NH1 = new bool [n];


        list<int>Q;
        list<int>::iterator it;
        G2.clear_vert(v);

        G2.reset_visited(visitado,n);
        G2.reset_visited(visitado2,n);

        visitado[v] = 1;
        visitado2[v] = 1;

        for(int i = 0; i < n; i++)
        {
            if(!visitado[i])  //a new graph to create
            {
                map<int,int> mymap; //map V_G -> V_H
                map<int,int> mymap2; //map V_H -> V_G

                numVisitados = 0;

                mymap.insert(pair<int,int>(v, numVisitados));
                mymap2.insert(pair<int,int>(numVisitados,v));

                visitado[i] = 1;
                numVisitados++;
                mymap.insert(pair<int,int>(i, numVisitados));
                mymap2.insert(pair<int,int>(numVisitados,i));

                Q.push_back(i);

                while(!Q.empty())
                {
                    u = Q.front(); Q.pop_front();

                    for(it = G2.adj[u].begin(); it != G2.adj[u].end(); ++it)
                        if(!visitado[*it])
                        {
                            visitado[*it] = 1;
                            numVisitados++;
                            mymap.insert(pair<int,int>(*it, numVisitados));
                            mymap2.insert(pair<int,int>(numVisitados,*it));
                            Q.push_back(*it);
                        }
                }

                //create the graph
                Graph H(numVisitados + 1);
                visitado2[i] = 1;
                Q.push_back(i);

                while(!Q.empty())
                {
                    u = Q.front(); Q.pop_front();

                    for(it = G.adj[u].begin(); it != G.adj[u].end(); ++it)
                    {
                        if(!H.existsEdge(mymap[u], mymap[*it]))
                            H.addEdge(mymap[u], mymap[*it]);

                        if(!visitado2[*it])
                        {
                            visitado2[*it] = 1;
                            Q.push_back(*it);
                        }
                    }
                }

                //update OH,NH,L for the graph
                for(int j = 0; j < H.get_num_vert(); j++)
                {
                    OH1[j] = OH[mymap2[j]];
                    NH1[j] = NH[mymap2[j]];
                    L1[j] = L[mymap2[j]];
                }
                L1[mymap[v]] = L[v] + G.degree(v) - H.degree(mymap[v]);

                //update LCut
                list <int> LCut_H1;

                for(int j = 0; j < H.get_num_vert(); j++)
                {
                    Cut_H1[j] = Cut_H[mymap2[j]];
                    if(Cut_H[mymap2[j]])
                        LCut_H1.push_back(j);
                }

                mymap.clear();
                mymap2.clear();

                sol += Solve_Graph(H, partial_time, OH1, NH1, L1, d, LCut_H1, Cut_H1, exact);
                time_solve += partial_time;
            }

        }

        sol = sol - k + 1;

        delete[] OH1;
        delete[] NH1;
        delete[] L1;
        delete[] Cut_H1;
        delete[] visitado;
        delete[] visitado2;
    }

    return sol;
}

int Solve_ModelCut(const Graph& G,float &time_solve, bool *OH, bool *NH, int *L, int d, int exact)
{
    int n = G.get_num_vert(), sol = 0;
    int *Cut_H = new int [n];
    list <int> LCut_H;

    time_solve = 0;

    G.detectCutH(Cut_H, LCut_H, OH, NH, L, d);    // devuelve lista CutH con cantidad de componentes
                                                            // conexas creadas asociadas a cada v \in CutH
    sol = Solve_Graph(G,time_solve, OH, NH,L,d, LCut_H, Cut_H, exact);

    delete []Cut_H;
    return sol;
}

int solveModels(Graph& G, float &time_solve, int &numb_OH,int &numb_NH, int d, int exact)
{
    int n = G.get_num_vert(), result = 0, partial_result = 0, *L;
    bool * visitado, **not_bridge, *OH, *NH, *OH1, *NH1;
    float time_H = 0;

    time_solve = 0;

    if(n < 3)
        {time_solve = 0; return 0;}

    not_bridge = new bool*[n];

    visitado = new bool[n];
    OH = new bool[n];
    NH = new bool[n];
    OH1 = new bool[n];
    NH1 = new bool[n];
    L = new int[n];

    for(int i = 0; i < n; i++)
    {
        not_bridge[i] = new bool[n];
        OH[i] = NH[i] = false;
        OH1[i] = NH1[i] = false;
        visitado[i] = false;
        L[i] = 0;
    }

    int num_bridges = G.detectBridges(not_bridge);

    findOH_NH(G,not_bridge,OH,NH,numb_OH,numb_OH,d);

    if(num_bridges == 0)    //The same Graph
    {
        result = Solve_ModelCut(G,time_H, OH, NH,L,d,exact);
        time_solve = time_H;
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            if(!NH[i] && !visitado[i])
            {
                Graph H;
                H = G.G_BFS(i,not_bridge,visitado,OH,NH, OH1, NH1,L);
                if(H.get_num_vert() == 1)
                    {partial_result = 1; time_H = 0;}
                else
                    partial_result = Solve_ModelCut(H,time_H, OH1, NH1,L,d, exact);
                    //partial_result = Solve_Model(H,time_H, OH1, NH1,L,d);

                result += partial_result;
                time_solve += time_H;
            }
        }
    }

    for (int i = 0; i < n; i++)
        delete []not_bridge[i];
    delete []not_bridge;

    delete []visitado;
    delete []OH;
    delete []NH;
    delete []OH1;
    delete []NH1;

    return result;
}

    int main(int argc, char *argv[])
    {
        int min_HV, numb_OH, numb_NH;
        float time_solve, real_time;
        clock_t start;

        if(argc < 4){
            cout<<"Faltam argumentos na chamada: dmbv nome_arquivo parametro_d leitor_instancia."<<endl;
            cout<<"Exemplo: dmbv Spd_RF2_80_133_1555.txt 2 0"<<endl;
            return 1;
        }

        int d = atoi(argv[2]);
        int inst = atoi(argv[3]);

        int exact = 0;
        if(argc >= 5){
            exact = atoi(argv[4]);
            exact = int(bool(exact));
        }

        cout<<"procesando ... "<<argv[1]<<" "<<d<<" "<<inst<<endl;
        Graph G;
        readInstance(inst,argv[1], G);

        //******* INIT TEST ****

        srand(G.get_num_vert());
        NumCut = 0;

        numb_OH = 0;
        time_solve = 0;

        start = clock();
        min_HV = solveModels(G,time_solve,numb_OH, numb_NH, d, exact);
        real_time = ((double)clock() - (double)start) / CLOCKS_PER_SEC;

        cout<<"\t"<< min_HV <<"\t"<<real_time<<"\t"<<time_solve<<"\t"<<NumCut<<endl;

        return 0;
    }

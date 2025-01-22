#include "Heuristic.h"
#include "RBEP.h"
#include "Roleta.h"
#include <fstream>
#include <filesystem>
#include <set>

std::string extractFileName(const std::string &filePath) {
    std::filesystem::path p(filePath);
    return p.filename().string();
}

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

/**
 * Função que realiza a busca da melhor árvore geradora para o grafo.
 *
 * @param g Endereço do grafo.
 * @param L Ponteiro para array de inteiro do tamanho da quantidade de vertices de g.
 * @param OH Ponteiro para array de booleanos do tamanho da quantidade de vertices de g (inicializado como falso).
 */
void Heuristic::ILS(igraph_t &g, int *L, bool *OH,  vector<float> &pagerank, const string &pathcomplete, 
                    int selectionConstruct, int selectionSearch, int orderingSearch, int orderingPetu)
{

    cout << endl;
    std::string path = extractFileName(pathcomplete);

    int n = igraph_vcount(&g);
    int u, v, e_1, e_2;

    igraph_adjlist_t adjlist; 
    igraph_adjlist_init(&g, &adjlist, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVert;

    if(selectionConstruct == 0)
        make_tree_ils(g,L,OH,adjlist); 
    else
        make_tree_rbep(g,L,OH,adjlist, pathcomplete, pagerank); 
    

    string step = "imagens";    

    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);
    
    if(numHV == 0)
        return;
    
    cout << "\tOrdering Local Search = " ;
    if(orderingSearch == 0)
        cout << "Degree" << endl;
    else
        cout << "Pagerank" << endl;

    cout << "\tSelection neighbours = " ;
    if(selectionSearch == 0)
        cout << "First" << endl;
    else
        cout << "Best" << endl;

    LocalSearch(g,L,OH,adjlist, pagerank, path, step, selectionSearch, orderingSearch);

    if(numHV == 0)
        return;

    bool **FE = new bool*[n];
    int *visitado = new int[n];

    for(int i = 0; i < n; i++)
    {
        FE[i] = new bool [n];

        for(int j = 0;j < n; j++)
            FE[i][j] = false;
    }

    igraph_t Tbest;
    igraph_copy(&Tbest, &T);
    int numHVbest = numHV;

    clock_t start;
    start = clock();
    float Time;

    cout << "\tOrdering Pertubation = " ;

    if(orderingPetu == 0)
        cout << "Degree" << endl;
    else
        cout << "Pagerank" << endl;

    do
    {
        Time = ((double)clock() - (double)start) / CLOCKS_PER_SEC;

        if(Time > time_Limit-1)
            break;

        if(orderingPetu == 0){
            if(!Pertubation_by_degree(g, L, OH, visitado, u, v, e_1, e_2, FE, adjlist, pagerank))
                break;
            else
                FE[e_1][e_2] = FE[e_2][e_1] = true;
            
        }else{
            if(!Pertubation_by_pagerank(g, L, OH, visitado, u, v, e_1, e_2, FE, adjlist, pagerank))
                break;
            else
                FE[e_1][e_2] = FE[e_2][e_1] = true;
            
        }

        LocalSearch(g,L,OH,adjlist, pagerank, path, step, selectionSearch, orderingSearch);

        if(numHV < numHVbest) //improvement
        {
            igraph_copy(&Tbest, &T);
            numHVbest = numHV;

            if(numHVbest == 0)
                break;

            for(int i = 0; i < n; i++)
            {
                adjVert = igraph_adjlist_get(&adjlist, i);
                for(int j = 0; j < igraph_vector_int_size(adjVert); j++)
                {
                    int aux = igraph_vector_int_e(adjVert, j);
                    FE[i][aux] = FE[aux][i]= false;
                }
            }

        }

    }while(true);

    igraph_copy(&T, &Tbest);
    numHV = 0;

    for(int i = 0; i < n; i++)
    {
        if(degree(T, i) + L[i] > d)
        {
            H[i] = true;
            numHV++;
        }

        else
            H[i] = false;
    }

    for(int i = 0; i < n; i++)
        delete []FE[i];

    delete []FE;
    delete []visitado;
    cout << endl;
    


}

/***************************LOCAL SEARCH*********************************************************************/
// Victor: Visa aprimorar a árvore inicial obtida através do procedimento make_tree. Seção 2.2.2 do artigo.
void Heuristic::LocalSearch(igraph_t &G, int *L, bool * OH, igraph_adjlist_t &adjlist, vector<float> &pagerank, 
                            const std::string &path, string &step, int selectionSearch, int orderingSearch)
{
    igraph_t T1;
    bool improvement, * LH;
    int n = igraph_vcount(&G);
    int *visitado;

    visitado = new int [n];
    LH = new bool [n];

    int cont  = 0;

    do
    {   
        improvement = Custom_Neighbor(G, T1, LH, L, OH, visitado, adjlist, pagerank, path, step, selectionSearch, orderingSearch);
        
        if(improvement)
            igraph_copy(&T, &T1);

        cont ++;

    }while(improvement);

    ///*******Update H and numHV********///

    numHV = 0;
    for(int i = 0; i < n; i++)
    {
        if(degree(T, i) + L[i] > d)
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

void orderer_vertex(igraph_t &T, vector< pair<float,int> > &S, vector<float> &pagerank, int n, bool *LH, int * L, bool *OH, int & min, int orderingSearch, int d){

    if(orderingSearch == 0){

        for(int i = 0; i < n; i++)
        {
            LH[i] = OH[i];
            if(degree(T, i) + L[i] > d && !OH[i]){
                S.push_back(make_pair(degree(T, i) + L[i], i) );
                min++;
            }
        }
    }
    else{

        for(int i = 0; i < n; i++)
        {
            LH[i] = OH[i];
            if(degree(T, i) + L[i] > d && !OH[i]){
                S.push_back(make_pair(pagerank[i], i) );
                min++;
            }
        }
    }

    sort(S.begin(), S.end());
}


bool Heuristic::Custom_Neighbor(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, 
                                vector<float> &pagerank, const std::string & path, string &step, int selectionSearch, int orderingSearch)
{
    /*** return the first tree with at least one less HV ***/
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    
    igraph_t bestT;
    igraph_copy(&bestT, &T);
    int minAtualDmbv = 0;
    
    orderer_vertex(T, S, pagerank, n, LH, L, OH, minAtualDmbv, orderingSearch, d);

    int qtdInicialDmbv = minAtualDmbv;
    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        // cout << "dbranch = " << v+1 << endl;
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);
            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);

                if(orderingSearch == 0)
                    FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                else
                    FindEdge_Pagerank(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist, pagerank); 


                // improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);
                
                if(e_1 != -1)   // and also e_2         
                {   
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }

                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
                {   
                    if(selectionSearch == 0)
                        break;
                    
                    int auxDmbv = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (degree(T1, i) + L[i] > d && !OH[i])
                        {
                            auxDmbv++;
                        }
                    }

                    if(auxDmbv < minAtualDmbv){
                        minAtualDmbv = auxDmbv;
                        igraph_copy(&bestT, &T1);
                    }
                }
            }

            if(selectionSearch == 0 && (degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d )))
            {
                S.clear();
                return true;
            }
            LH[v] = true;
        }
    }

    igraph_copy(&T1, &bestT);
    igraph_destroy(&bestT);
    if((minAtualDmbv  < qtdInicialDmbv) && selectionSearch != 0)
        return true;

    return false;
}



/*
    Variação do algoritmo FirstBestNeighbor implementado por moreno na tese.
    Busca a melhor modificacao, sempre passa por todos os vertices d-BRANCH.
    Ordenação dos vértices d-branch pelo menor grau.
*/
bool Heuristic::BestNeighbor_by_degree(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string & path, string &step)
{
    /*** return the first tree with at least one less HV ***/
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    igraph_t bestT;
    igraph_copy(&bestT, &T);
    int minAtualDmbv = 0;

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i]){
            S.push_back(make_pair(degree(T, i) + L[i], i) );
            minAtualDmbv++;
        }
    }

    int qtdInicialDmbv = minAtualDmbv;
    
    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);
            // cout << "neighbors of:" << v + 1 << endl;
            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                // cout << v + 1 << "--" << u + 1 << endl;
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                
                improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);
                
                if(e_1 != -1)   // and also e_2         
                {   
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }

                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
                {   
                    int auxDmbv = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (degree(T1, i) + L[i] > d && !OH[i])
                        {
                            auxDmbv++;
                        }
                    }

                    if(auxDmbv < minAtualDmbv){
                        minAtualDmbv = auxDmbv;
                        igraph_copy(&bestT, &T1);
                    }
                }
            }
            LH[v] = true;
        }
    }

    igraph_copy(&T1, &bestT);
    igraph_destroy(&bestT);
    if(minAtualDmbv  < qtdInicialDmbv)
        return true;

    return false;
}

/*
    Variação do algoritmo FirstBestNeighbor implementado por moreno na tese.
    Busca a melhor modificacao, sempre passa por todos os vertices d-BRANCH.
    Ordenação dos vértices d-branch pelo menor pagerank.
*/
bool Heuristic::BestNeighbor_by_pagerank(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string & path, string &step)
{
    /*** return the first tree with at least one less HV ***/
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    igraph_t bestT;
    igraph_copy(&bestT, &T);
    int minAtualDmbv = 0;

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i]){
            S.push_back(make_pair(pagerank[i], i) );
            minAtualDmbv++;
        }
    }

    int qtdInicialDmbv = minAtualDmbv;
    
    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);
            // cout << "neighbors of:" << v + 1 << endl;
            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                // cout << v + 1 << "--" << u + 1 << endl;
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                
                improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);
                
                if(e_1 != -1)   // and also e_2         
                {   
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }

                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
                {   
                    int auxDmbv = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (degree(T1, i) + L[i] > d && !OH[i])
                        {
                            auxDmbv++;
                        }
                    }

                    if(auxDmbv < minAtualDmbv){
                        minAtualDmbv = auxDmbv;
                        igraph_copy(&bestT, &T1);
                    }
                }
            }
            LH[v] = true;
        }
    }

    igraph_copy(&T1, &bestT);
    igraph_destroy(&bestT);
    if(minAtualDmbv  < qtdInicialDmbv)
        return true;

    return false;
}

/*
    Algoritmo original do FirstBestNeighbor implementado por moreno na tese.
*/
bool Heuristic::FirstBestNeighbor_by_degree(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string &path, string &step)
{
    /*** return the first tree with at least one less HV ***/
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i])
            S.push_back(make_pair(degree(T, i) + L[i], i) );
    }

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;
    
    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);

            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                
                improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);
                
                if(e_1 != -1)   // and also e_2         
                {
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }
                
                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d )){
                    break;
                }
            }

            if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
            {
                S.clear();
                return true;
            }

            LH[v] = true;
        }
    }

    return false;
}


/*
    Variação do algoritmo FirstBestNeighbor implementado por moreno na tese
    Modificando a ordernacao dos vertices d-branch;
    Vertices com menor pagerank vao ser analisados primeiros;
*/
bool Heuristic::FirstBestNeighbor_by_pagerank(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string &path, string &step)
{
    /*** return the first tree with at least one less HV ***/
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;
    
    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i]){
            S.push_back(make_pair(pagerank[i], i) );
        }
    }

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);

            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 

                improvement++;
                // igraph_t auxT;
                // igraph_copy(&auxT, &T1);
                // print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);

                if(e_1 != -1)   // and also e_2         
                {
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }
                if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
                    break;
            }

            if(degree(T1, v)+L[v] <= d || (flag && degree(T1, u)+L[u] <= d ))
            {
                S.clear();
                return true;
            }

            LH[v] = true;
        }
    }

    return false;
}

/*
    Variação do algoritmo FirstBestNeighbor implementado por moreno na tese.
    Aceita como melhoria quando o grau do vertice d-Branch analisado é diminuido em 1.
    Ordernação dos vértices d-branch por menor grau
    TA DANDO ERRO
*/
bool Heuristic::FirstBestNeighborStar_by_degree(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string &path, string &step)
{
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i])
            S.push_back(make_pair(degree(T, i) + L[i], i) );
    }

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        int grauV = -1;
        int grauU = -1;
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);

            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                grauU = degree(T1, u);
                grauV = degree(T1, v);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                
                improvement++;
                igraph_t auxT;
                igraph_copy(&auxT, &T1);
                print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);

                if(e_1 != -1)   // and also e_2         
                {
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }
                if(((degree(T1, v)+L[v] < grauV) || (degree(T1, u)+L[u] < grauU)) && ((degree(T1, e_1)+L[e_1] <= d) && (degree(T1, e_2)+L[e_2] <= d))){
                    break;
                }
            }
     
            if((degree(T1, v)+L[v] < grauV) || (degree(T1, u)+L[u] < grauU) && ((degree(T1, e_1)+L[e_1] <= d) && (degree(T1, e_2)+L[e_2] <= d)))
            {
                S.clear();
                return true;
            }

            LH[v] = true;
        }
    }

    return false;
}


/*
    Variação do algoritmo FirstBestNeighbor implementado por moreno na tese.
    Aceita como melhoria quando o grau do vertice d-Branch analisado é diminuido em 1.
    Ordernação dos vértices d-branch por menor pagerank
    TA DANDO ERRO
*/
bool Heuristic::FirstBestNeighborStar_by_pagerank(igraph_t &G, igraph_t &T1, bool *LH, int * L, bool *OH, int *visitado, igraph_adjlist_t &adjlist, vector<float> &pagerank, const std::string &path, string &step)
{
    bool flag;
    int n = igraph_vcount(&G);
    int u, v, e_1, e_2;

    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables
    igraph_copy(&T1, &T);

    for(int i = 0; i < n; i++)
    {
        LH[i] = OH[i];
        if(degree(T, i) + L[i] > d && !OH[i])
            S.push_back(make_pair(pagerank[i], i) );
    }

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    int improvement = 0;

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {   
        v = (*itS).second; 
        int grauV = -1;
        int grauU = -1;
        if(degree(T1, v)+L[v] > d)
        {
            igraph_neighbors(&T1, &AdjV, v, IGRAPH_OUT);

            for(int j = 0; j < igraph_vector_size(&AdjV); j++)
            {
                u = igraph_vector_e(&AdjV, j);
                grauU = degree(T1, u);
                grauV = degree(T1, v);
                flag = degree(T1, u)+L[u] > d ? true : false;
                RemoveEdge(T1, u, v, visitado);
                FindEdge_Degree(G, T1, LH, L, OH, visitado, u, v, e_1, e_2, adjlist); 
                
                improvement++;
                igraph_t auxT;
                igraph_copy(&auxT, &T1);
                print_graphviz_neighbor(G, auxT, e_1, e_2, v, u, path, step, improvement);

                if(e_1 != -1)   // and also e_2         
                {
                    removeEdge(T1, u,v);
                    addEdge(T1, e_1, e_2);
                }
                cout << "loop" << endl;
                if(((degree(T1, v)+L[v] < grauV) || (degree(T1, u)+L[u] < grauU)) && ((degree(T1, e_1)+L[e_1] >= d) || (degree(T1, e_2)+L[e_2] >= d))){
                    break;
                }

                // if(((degree(T1, u)+L[u] < grauU)) && ){
                //     break;
                // }
            }
     
            if(((degree(T1, v)+L[v] < grauV) || (degree(T1, u)+L[u] < grauU)) && ((degree(T1, e_1)+L[e_1] >= d) || (degree(T1, e_2)+L[e_2] >= d)))
            {
                S.clear();
                return true;
            }

            LH[v] = true;
        }
    }

    return false;
}

void Heuristic::RemoveEdge(igraph_t &T1, int u, int v, int *visitado)
{
    list <int> S;
    int x; 
    int n = igraph_vcount(&T);
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    for(int i = 0; i < n; i++)
        visitado[i] = -1;

    //avoiding enter in v
    visitado[v] = u;

    visitado[u] = u;
    S.push_front(u);

    while(!S.empty())
    {
        x = S.front(); S.pop_front();
        igraph_neighbors(&T1, &AdjV, x, IGRAPH_OUT);
        
        for(int j = 0; j < igraph_vector_size(&AdjV); ++j)
        {
            int aux = igraph_vector_e(&AdjV, j);
            if(visitado[aux] != u)
              {
                  visitado[aux] = u;
                  S.push_front(aux);
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
        igraph_neighbors(&T1, &AdjV, x, IGRAPH_OUT);

        for(int j = 0; j < igraph_vector_size(&AdjV); ++j)
        {
            int aux = igraph_vector_e(&AdjV, j);
            if(visitado[aux] != v)
              {
                  visitado[aux] = v;
                  S.push_front(aux);
              }
        }
    }
    //the correct value
    visitado[u] = u;
}

// Victor: Tenta encontrar uma aresta diferente de {u, v} que conecta as subárvores Tu e Tv de modo que T tenha menos vértices d-branch.
void Heuristic::FindEdge_Degree(igraph_t &G, igraph_t &T1, bool *LH, int *L, bool * OH,int *visitado, int u, int v, int &e_1,int &e_2, igraph_adjlist_t &adjlist)
{
    int i, j, optionIJ = CASE_33, optionI = CASE_3, n = igraph_vcount(&G);
    int edge[6][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
    int weight[6]= {2*n, 2*n, 2*n, 2*n, 2*n, 0};
    int wIJ;
    igraph_vector_int_t *adjVert;
    
    e_1 = e_2 = -1;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            bool explore = false;

            if(LH[i])
                {explore = true; optionI = CASE_1;}

            if(optionIJ >= CASE_12 && (degree(T1, i)+L[i] < d || ((i == u || i == v) && degree(T1, i)+L[i]-1 < d)) )
                {explore = true; optionI = CASE_2;}

            if(!LH[i] && optionIJ >= CASE_13 && (degree(T1, i)+L[i] > d || ((i == u || i == v) && degree(T1, i)+L[i]-1 > d)) )
                {explore = true; optionI = CASE_3;}

            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int k = 0; k < igraph_vector_int_size(adjVert) && explore; ++k)
            {
                j = igraph_vector_int_e(adjVert, k);
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
                                            wIJ = degree(G, i) + degree(G, j) - n*OH[i] - n*OH[j];
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
                                                wIJ = degree(G, i) - n*OH[i] + degree(G, j);
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
                                                wIJ = degree(G, i) - n*OH[i] - degree(T1, j);
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

                        if( optionIJ >= CASE_12 && (degree(T1, j)+L[j] < d || ( (j == u || j == v) && degree(T1, j)+L[j]-1 < d)) )  // j = CASE_2
                        {
                            switch(optionI) 
                            {
                              case CASE_1:  if(optionIJ >= CASE_12) //CASE21
                                            {
                                                optionIJ = CASE_12;
                                                wIJ = degree(G, i) + degree(G, j) - n*OH[j] ;
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
                                                wIJ = degree(G, i) + degree(G, j);
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
                                                wIJ = degree(G, i) - degree(T1, j);
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

                        if(!LH[j] && optionIJ >= CASE_13 && (degree(T1, j)+L[j] > d || ((j == u || j == v) && degree(T1, j)+L[j]-1 > d)) )  // j = CASE_3
                        {
                            switch(optionI)
                            {
                              case CASE_1:  if(optionIJ >= CASE_13) //CASE_31
                                            {
                                                optionIJ = CASE_13; 
                                                wIJ = degree(G, j) - n*OH[j] - degree(T1, i);
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
                                                wIJ = degree(G, j) - degree(T1, i);
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
                                                wIJ = degree(T1, j) + degree(T1, i);
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

void Heuristic::FindEdge_Pagerank(igraph_t &G, igraph_t &T1, bool *LH, int *L, bool * OH,int *visitado, 
                                    int u, int v, int &e_1,int &e_2, igraph_adjlist_t &adjlist, vector<float> &pagerank)
{
    int i, j, optionIJ = CASE_33, optionI = CASE_3, n = igraph_vcount(&G);
    int edge[6][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
    float weight[6]= {(float) 2*n, (float) 2*n, (float) 2*n, (float) 2*n, (float) 2*n, (float) 0};
    float wIJ;
    igraph_vector_int_t *adjVert;
    
    e_1 = e_2 = -1;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            bool explore = false;

            if(LH[i])
                {explore = true; optionI = CASE_1;}

            if(optionIJ >= CASE_12 && (degree(T1, i)+L[i] < d || ((i == u || i == v) && degree(T1, i)+L[i]-1 < d)) )
                {explore = true; optionI = CASE_2;}

            if(!LH[i] && optionIJ >= CASE_13 && (degree(T1, i)+L[i] > d || ((i == u || i == v) && degree(T1, i)+L[i]-1 > d)) )
                {explore = true; optionI = CASE_3;}

            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int k = 0; k < igraph_vector_int_size(adjVert) && explore; ++k)
            {
                j = igraph_vector_int_e(adjVert, k);
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
                                            // wIJ = degree(G, i) + degree(G, j) - n*OH[i] - n*OH[j];
                                            wIJ = pagerank[i] + pagerank[j];
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
                                                // wIJ = degree(G, i) - n*OH[i] + degree(G, j);
                                                wIJ = pagerank[i] + pagerank[j];
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
                                                // wIJ = degree(G, i) - n*OH[i] - degree(T1, j);
                                                wIJ = pagerank[i] + pagerank[j];
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

                        if( optionIJ >= CASE_12 && (degree(T1, j)+L[j] < d || ( (j == u || j == v) && degree(T1, j)+L[j]-1 < d)) )  // j = CASE_2
                        {
                            switch(optionI) 
                            {
                              case CASE_1:  if(optionIJ >= CASE_12) //CASE21
                                            {
                                                optionIJ = CASE_12;
                                                // wIJ = degree(G, i) + degree(G, j) - n*OH[j] ;
                                                wIJ = pagerank[i] + pagerank[j];
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
                                                // wIJ = degree(G, i) + degree(G, j);
                                                wIJ = pagerank[i] + pagerank[j];

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
                                                // wIJ = degree(G, i) - degree(T1, j);
                                                wIJ = pagerank[i] + pagerank[j];

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

                        if(!LH[j] && optionIJ >= CASE_13 && (degree(T1, j)+L[j] > d || ((j == u || j == v) && degree(T1, j)+L[j]-1 > d)) )  // j = CASE_3
                        {
                            switch(optionI)
                            {
                              case CASE_1:  if(optionIJ >= CASE_13) //CASE_31
                                            {
                                                optionIJ = CASE_13; 
                                                // wIJ = degree(G, j) - n*OH[j] - degree(T1, i);
                                                wIJ = pagerank[i] + pagerank[j];

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
                                                // wIJ = degree(G, j) - degree(T1, i);
                                                wIJ = pagerank[i] + pagerank[j];

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
                                                // wIJ = degree(T1, j) + degree(T1, i);
                                                wIJ = pagerank[i] + pagerank[j];

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
// Victor: Visa substituir uma aresta de T que tem pelo menos um vértice 
// d-branch não obrigatório por outra aresta {u', v'} != {u, v}, com u' ∈ Tu e v' ∈ Tv, 
// criando outro vértice d-branch (u' or v'). Seção 2.2.3 do artigo.
bool Heuristic::Pertubation_by_degree(igraph_t &G, int * L, bool *OH, int *visitado, int &u,int &v,int &e_1,int &e_2, bool **FE, igraph_adjlist_t &adjlist, vector<float> &pagerank)
{
    int n = igraph_vcount(&G);

    vector< pair<int,int> > S;
    vector< pair<int,int> > :: iterator itS;
    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables

    for(int i = 0; i < n; i++) 
        if(degree(T, i)+L[i] > d && !OH[i])
            S.push_back( make_pair(degree(G, i)+L[i], i) );

    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {
        v = (*itS).second;
        igraph_neighbors(&T, &AdjV, v, IGRAPH_OUT);

        for(int j = 0; j < igraph_vector_size(&AdjV); j++)
        {
            u = igraph_vector_e(&AdjV, j);
            RemoveEdge(T, u, v, visitado);
            FindEdgePert_by_degree(G, L, OH, visitado, u, v, e_1, e_2, FE, adjlist); 

            if(e_1 != -1)   // and also e_2
            {
                removeEdge(T, u,v);
                addEdge(T, e_1, e_2);
                return true;
            }
        }
    }
    u = v = e_1 = e_2 = -1;
    return false;
}

bool Heuristic::Pertubation_by_pagerank(igraph_t &G, int * L, bool *OH, int *visitado, int &u,int &v,int &e_1,int &e_2, bool **FE, igraph_adjlist_t &adjlist, vector<float> &pagerank)
{
    int n = igraph_vcount(&G);

    vector< pair<float,int> > S;
    vector< pair<float,int> > :: iterator itS;

    igraph_vector_t AdjV;
    igraph_vector_init(&AdjV, 0);

    //initialize the variables

    for(int i = 0; i < n; i++) 
        if(degree(T, i)+L[i] > d && !OH[i])
            S.push_back(make_pair(pagerank[i], i) );


    if(S.empty())
        return false;

    sort(S.begin(), S.end());

    //exploring S
    for(itS = S.begin(); itS != S.end(); ++itS)
    {
        v = (*itS).second;
        igraph_neighbors(&T, &AdjV, v, IGRAPH_OUT);

        for(int j = 0; j < igraph_vector_size(&AdjV); j++)
        {
            u = igraph_vector_e(&AdjV, j);
            RemoveEdge(T, u, v, visitado);
            FindEdgePert_by_pagerank(G, L, OH, visitado, u, v, e_1, e_2, FE, adjlist, pagerank); 

            if(e_1 != -1)   // and also e_2
            {
                removeEdge(T, u,v);
                addEdge(T, e_1, e_2);
                return true;
            }
        }
    }
    u = v = e_1 = e_2 = -1;
    return false;
}

// Victor: Responsável por encontrar uma aresta {u', v'} != {u, v}, com u' ∈ Tu e v' ∈ Tv, 
// criando outro vértice d-branch (u' or v'). Essa aresta substituirá uma aresta {u, v} de T. 
// Cada vértice v é analisado em ordem crescente conforme o valor dG(v) + l(v). 
// Dentre todas as arestas possíveis, deve ser selecionada a aresta {u', v'} que reconectará 
// a árvore e criará exatamente um vértice d-branch (i.e. dT(u') = d or dT(v') = d), e que minimiza: {u', v'} = argmin{i,j}{dG(i) + dG(j)}
void Heuristic::FindEdgePert_by_degree(igraph_t &G, int *L, bool * OH,int *visitado, int u, int v, int &e_1,int &e_2, bool **FE, igraph_adjlist_t &adjlist)
{
    int i, j, n = igraph_vcount(&G);
    list<int>::iterator it;
    int bestI, bestJ, bestW;
    igraph_vector_int_t *adjVert;

    bestI = bestJ = -1;
    bestW = 2*n;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int k = 0; k < igraph_vector_int_size(adjVert); ++k) 
            {
                j = igraph_vector_int_e(adjVert, k);
                if(visitado[j] == v)    //the subtree of v 
                {
                    if( !((i == u && j == v) || (i == v && j == u)) && !FE[i][j] 
                    && ((degree(T, i)+L[i] == d && degree(T, j)+L[j] != d) || (degree(T, j)+L[j] == d && degree(T, i)+L[i] != d)) )
                    {
                        ///select the best candidate edge
                        if(degree(G, i) + degree(G, j) < bestW )
                        {
                            bestI = i;
                            bestJ = j;
                            bestW = degree(G, i)+degree(G, j);
                        }

                    }

                }
            }
        }

    }

    e_1 = bestI;
    e_2 = bestJ;
}

void Heuristic::FindEdgePert_by_pagerank(igraph_t &G, int *L, bool * OH,int *visitado, int u, int v, int &e_1,int &e_2, bool **FE, igraph_adjlist_t &adjlist, vector<float> &pagerank)
{
    int i, j, n = igraph_vcount(&G);
    list<int>::iterator it;
    int bestI, bestJ;
    double bestW;
    igraph_vector_int_t *adjVert;

    bestI = bestJ = -1;
    bestW = 100;

    for(i = 0; i < n; i++)
    {
        if(visitado[i] == u)    //the subtree of u
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int k = 0; k < igraph_vector_int_size(adjVert); ++k) 
            {
                j = igraph_vector_int_e(adjVert, k);
                if(visitado[j] == v)    //the subtree of v 
                {
                    if( !((i == u && j == v) || (i == v && j == u)) && !FE[i][j] 
                    && ((degree(T, i)+L[i] == d && degree(T, j)+L[j] != d) || (degree(T, j)+L[j] == d && degree(T, i)+L[i] != d)) )
                    {
                        ///select the best candidate edge
                        if(pagerank[i] + pagerank[j] < bestW)
                        {
                            bestI = i;
                            bestJ = j;
                            bestW = pagerank[i] + pagerank[j];
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
    ofstream fout_x("x_matrix_ils.txt");
    ofstream f_out_y("ILS_Sol_vector.txt");

    int n = igraph_vcount(&T); 
    int v, u;
    igraph_adjlist_t adjlistG;
    igraph_adjlist_init(&T, &adjlistG, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVertG;

    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            x[i][j] = false;

    for(int i = 0; i < n; i++)
        {
            y[i] = H[i];
            z[i] = 0;
        }

    for(int i=0; i< n; i++)
      f_out_y<<y[i]<<" ";
    f_out_y<<endl;

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


    //list<int>::iterator it;

    int num_visit;
    
    while(!Q.empty())
    {
        v = Q.front(); Q.pop_front();
        num_visit = 0;
        adjVertG = igraph_adjlist_get(&adjlistG, v);
        int end = igraph_vector_int_size(adjVertG);
        /*for(it = T.adj[v].begin(); it != T.adj[v].end(); ++it)*/
        for(int it = 0; it != end; ++it)
        {
            //u = *it;
            u = igraph_vector_int_e(adjVertG, it);            
            
            if(!visited[u])
            {
                num_visit++;
                visited[u] = true;
                z[u] = z[v] + 1;
                x[v][u] = true;
                fout_x << v << " " << u <<endl;
                Q.push_back(u);
            }
        }
    }
}

void Heuristic::printH2()
{
    for(int i = 0; i < igraph_vcount(&T); i++)
        if(H[i])
          {
              cout<<"d("<<i<<") = "<<degree(T, i)<<endl;
          }
    cout<<endl; 
}

void Heuristic::printH()
{
    for(int i = 0; i < igraph_vcount(&T); i++)
        if(H[i])
            cout<<i<<" ";
    cout<<endl;
}

/**
 * Função que realiza a construção da solucao inicial.
 * Tópico 2.2.1 da tese Moreno.
 *
 * @param G Endereço do grafo.
 * @param L Ponteiro para array de inteiro do tamanho da quantidade de vertices de g.
 * @param OH Ponteiro para array de booleanos do tamanho da quantidade de vertices de g (inicializado como falso).
 * @param adjlist Ponteiro de lista de adjacentes criado pela biblioteca igraph.
 */ 

void Heuristic::make_tree_rbep(igraph_t &g_aux, int*L, bool *OH, igraph_adjlist_t &adjlist, const string &path, vector<float> &pagerank){
    Grafo GREBP = Grafo::CopiarIGraph(g_aux, pagerank);
    Grafo TREBP;
    Grafo Tmin;
    cout << "\tConstruct = R-BEP" << endl;;
    
    vector<int> BT;
    vector<int> BTv;
    
    int BTMin = 1001;
    int BTMax = 0;
    
    double Time = 0.0;
    string verificado = "True";

    int seed = 100;
    std::mt19937_64 gen(seed);

    for(int i=0; i<100; i++)
    {
        double time_spent = 0.0;
        clock_t start = clock();

        RBEP alg = RBEP(GREBP);

        alg.Oliveira(gen);

        clock_t end = clock();
        time_spent += (double)(end - start) / CLOCKS_PER_SEC;

        Time += time_spent;

        TREBP = alg.ObterArvore();

            if(i == 0)
                Tmin = alg.ObterArvore();
        
        BT = alg.ObterBranches();
        BTv.push_back(BT.size());

        if(BT.size() < BTMin){
            BTMin = BT.size();
            Tmin = alg.ObterArvore(); 
        }

        if(BT.size() > BTMax)
            BTMax = BT.size();
        if(TREBP.ValidarArvore() != true || TREBP.V.size() != GREBP.V.size())
            verificado = "False";

    }

    // float soma = 0.0;
    // for(int i : BTv)
    //    soma = soma + i;

    // ofstream RBEPm("output/teste-rbep.csv", std::ios_base::app);
    // RBEPm << path <<  ";"<< GREBP.n << ";" << GREBP.m << ";" << BTMin<<";"<<BTMax<< ";"<<soma/100 << ";"<< endl;
    // RBEPm.close();
    int n = Tmin.n;
    int m = Tmin.m;

    if(H)
        delete []H;

    H = new bool [n];

    igraph_t T1;
    igraph_empty(&T1, n, 0);
    
    for(int v = 0; v < n; v++) {
        for(int u : Tmin.Adjacentes(v)) {
            if (!existsEdge(T1, v, u) && (v != u)) {
                igraph_add_edge(&T1, v, u);
            }
        }
    }

    igraph_copy(&T, &T1);
    numHV = 0;

    for(int i = 0; i < n; i++)
    {   
        if(degree(T, i) + L[i] > d)
        {
            H[i] = true;
            numHV++;
        }
        else
            H[i] = false;
    }  

}

void Heuristic::make_tree_ils(igraph_t &G, int*L, bool *OH, igraph_adjlist_t &adjlist)
{   
    cout << "\tConstruct = ILS" << endl;
    int n = igraph_vcount(&G);
    int *component = new int[n];
    int *length_component = new int[n];

    for(int i = 0; i < n; i++)
    {
        component[i] = i;
        length_component[i] = 1;
    }

    int m = 0;
    int selection = 1;
    igraph_empty(&T, n, 0);

    if(H)
        delete []H;

    H = new bool [n];

    int option = selection;
    while( m < n-1 )
    {
        //cout << "Selecao = " << selection << endl;
        switch(selection)
        {
            case 1: if(!Select_1(G, component, length_component, m, L, OH, adjlist))
                        selection = 3;
                    break;

            case 2: if(Select_2(G, component, length_component, m, L, OH, adjlist))
                        selection = 3;
                    else
                        selection = 5;
                    break;

            case 3: if(Select_3(G, component, length_component, m, L, OH, adjlist))
                        selection = 3;
                    else
                        selection = 4;
                    break;

            case 4: if(Select_4(G, component, length_component, m, L, OH, adjlist))
                        selection = 3;
                    else
                        selection = 2;
                    break;

            case 5: if( Select_5(G, component, length_component, m, L, OH, adjlist))
                        selection = 3;
                    else
                        selection = 6;
                    break;

            case 6: Select_6(G, component, length_component, m, L, OH, adjlist);
                    selection = 3;
                    break;
        }

        if(igraph_ecount(&T) < m)
        {
            cout<<"Error en selection "<<option<<endl;
        }
        option = selection;

    }

    numHV = 0;

    for(int i = 0; i < n; i++)
    {
        if(degree(T, i) + L[i] > d)
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

/**
 * @param G Endereço do grafo.
 * @param component Ponteiro de um vetor de inteiros dos indices dos vértices, se o grafo de 10 vertices esse vetor tem os numeros de 0 a 9 nos seus indices.  
 * @param length_component Ponteiro de um vetor de inteiros com o tamanho igual ao número de vértices de G (inicializado com o valor 1).  
 * @param m Váriavel int que reprensenta o vertice atual na árvore T.
 * @param L Ponteiro de um vetor de inteiros com o tamanho igual a quantidade de vertices de G (inicializado com o valor 0).
 * @param OH Ponteiro para array de booleanos do tamanho da quantidade de vertices de G (inicializado como falso).
 * @param adjlist Ponteiro de lista de adjacentes criado pela biblioteca igraph.
 * 
 * @return FALSE se foi adicionado alguma aresta em T ou TRUE se nada for modificado em T. 
 */ 
bool Heuristic::Select_1(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int min_u = -1, min_v = -1, min_weight = INT_MAX;
    igraph_vector_int_t *adjVert;

    for(int i = 0; i < n; i++)//Passa por todos os vertices.
    {
        if(degree(T, i) + L[i] == 0)//Se em T o grau desse vertice for 0.
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //Passo por todos os adjacentes do vértice de indice i.
            {
                int aux = igraph_vector_int_e(adjVert, j);//retorna o nome do vertice adjacente?
                if(degree(T, aux) + L[aux] != d)//se o vertice adjacente em T for diferente de 2, nesse caso.
                {
                    if(component[aux] != component[i]) // confere se eles são o mesmo
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] - OH[i]*n - OH[aux]*n < min_weight)
                        {
                            min_u = i;
                            min_v = aux;
                            min_weight = degree(G, i) + L[i] + degree(G, aux) + L[aux] - OH[i]*n - OH[aux]*n;
                        }
                }
            }
        }
        else
        {
                if(degree(T, i) + L[i] != d)
                {
                adjVert = igraph_adjlist_get(&adjlist, i);
                for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
                {
                    int aux = igraph_vector_int_e(adjVert, j);
                    if(degree(T, aux) == 0)
                    {
                        if(component[aux] != component[i])
                            if(degree(G, i)+L[i] + degree(G, aux)+L[aux] - OH[i]*n - OH[aux]*n < min_weight)
                            {
                                min_u = aux;
                                min_v = i;
                                min_weight = degree(G, i)+L[i] + degree(G, aux)+L[aux] - OH[i]*n - OH[aux]*n;
                            }
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

        addEdge(T, min_u, min_v);
        m++;

        return true;
    }
    return false;
}

bool Heuristic::Select_2(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int min_u = -1, min_v = -1, min_weight = INT_MAX, aux, i;
    igraph_vector_int_t *adjVert;

    for(i = 0; i < n; i++)
    {
        if(degree(T, i)+L[i] < d)
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
            {
                int aux = igraph_vector_int_e(adjVert, j);
                if(degree(T, aux)+L[aux] < d)
                {
                    if(component[aux] != component[i])
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] - OH[i]*n - OH[aux]*n < min_weight)
                        {
                            min_u = i;
                            min_v = aux;
                            min_weight = degree(G, i) + L[i] + degree(G, aux) + L[aux] - OH[i]*n - OH[aux]*n;
                        }
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

        addEdge(T, min_u, min_v);
        m++;

        return true;
    }
    return false;
}

bool Heuristic::Select_3(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int max_u = -1, max_v = -1, max_weight = 0, aux, i;
    igraph_vector_int_t *adjVert;

    for(i = 0; i < n; i++)
    {
        if(degree(T, i)+L[i] < d)
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
            {
                int aux = igraph_vector_int_e(adjVert, j);
                if(degree(T, aux)+L[aux] > d)
                {
                    if(component[aux] != component[i])
                        if(degree(G, aux)+L[aux]+OH[aux]*n > max_weight)
                        {
                            max_u = i;
                            max_v = aux;
                            max_weight = degree(G, aux)+L[aux]+OH[aux]*n;
                        }
                }
            }
        }

    }

    if(max_u != -1)
    {
        if(degree(T, max_u) == 0)
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

        addEdge(T, max_u, max_v);
        m++;

        return true;
    }
    return false;
}

bool Heuristic::Select_4(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int max_u = -1, max_v = -1, max_weight = 0, aux, i;
    igraph_vector_int_t *adjVert;

    for(i = 0; i < n; i++)
    {
        if(degree(T, i) + L[i] > d)
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
            {
                int aux = igraph_vector_int_e(adjVert, j);
                if(degree(T, aux)+L[aux] > d)
                {
                    if(component[aux] != component[i])
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n > max_weight)
                        {
                            max_u = i;
                            max_v = aux;
                            max_weight = degree(G, i) + L[i] + degree(G, aux) + L[aux]  + OH[i]*n + OH[aux]*n;
                        }
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

        addEdge(T, max_u, max_v);
        m++;

        return true;
    }
    return false;
}

bool Heuristic::Select_5(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int max_u = -1, max_v = -1, max_weight = 0, aux, i;
    igraph_vector_int_t *adjVert;

    for(i = 0; i < n; i++)
    {
        if(degree(T, i) + L[i] > d)
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
            {
                int aux = igraph_vector_int_e(adjVert, j);
                if(degree(T, aux)+L[aux] == d)
                {
                    if(component[aux] != component[i])
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n > max_weight)
                        {
                            max_u = i;
                            max_v = aux;
                            max_weight = degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n;
                        }
                }
            }
        }

    }

    if(max_u != -1)
    {
        if(degree(T, max_u) == 0)
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

        addEdge(T, max_u, max_v);
        m++;

        return true;
    }
    return false;
}

void Heuristic::Select_6(igraph_t &G, int *component, int *length_component, int &m, int *L, bool *OH, igraph_adjlist_t &adjlist)
{
    int n = igraph_vcount(&G);
    int max_u1 = -1, max_v1 = -1, max_weight1 = 0, max_u2 = -1, max_v2 = -1, max_weight2 = 0, aux, i;
    igraph_vector_int_t *adjVert;

    for(i = 0; i < n; i++)
    {
        if(degree(T, i)+L[i] == d)
        {
            adjVert = igraph_adjlist_get(&adjlist, i);
            for(int j = 0; j < igraph_vector_int_size(adjVert); ++j) //foreach adjacent
            {
                int aux = igraph_vector_int_e(adjVert, j);
                if(degree(T, aux)+L[aux] < d)
                {
                    if(component[aux] != component[i])
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n > max_weight1)
                        {
                            max_u1 = i;
                            max_v1 = aux;
                            max_weight1 = degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n;
                        }
                }
                else    //T.deg[*it] + L[*it] == d
                {
                    if(component[aux] != component[i])
                        if(degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n > max_weight2)
                        {
                            max_u2 = i;
                            max_v2 = aux;
                            max_weight2 = degree(G, i) + L[i] + degree(G, aux) + L[aux] + OH[i]*n + OH[aux]*n;
                        }
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
        addEdge(T, max_u1, max_v1);
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

        addEdge(T, max_u2, max_v2);

    }

    m++;

}

void Heuristic::absorve(int *component, int min_u, int min_v, int *length_component)
{
    int n = igraph_vcount(&T);
    length_component[component[min_u]] += length_component[component[min_v]];
    int comp_min_v = component[min_v];

    for(int i = 0; i < n; i++)
        if(component[i] == comp_min_v)
            {
                component[i] = component[min_u];
                length_component[i] = 0;
            }
}


void Heuristic::print_graphviz(std::ofstream& output, igraph_t &G) {
    igraph_adjlist_t adjlistT;
    igraph_adjlist_init(&T, &adjlistT, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVertT;

    igraph_adjlist_t adjlistG;
    igraph_adjlist_init(&G, &adjlistG, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVertG;

    output << "graph G {" << endl;

    int nodAtual = 0;
    for (int i = 0; i < igraph_vcount(&T); i++) {
        nodAtual = i + 1;

        adjVertT = igraph_adjlist_get(&adjlistT, i);
        adjVertG = igraph_adjlist_get(&adjlistG, i);

        for (int j = 0; j < igraph_vector_int_size(adjVertG); j++) {
            int auxG = igraph_vector_int_e(adjVertG, j) + 1;
            int auxT = -1;
                for (int h = 0; h < igraph_vector_int_size(adjVertT); h++){
                    int auxT_aux = igraph_vector_int_e(adjVertT, h) + 1;
                    if(auxG == auxT_aux){
                        auxT = auxT_aux;
                        break;
                    }
                }
            if(auxT == -1){
                if(auxG > nodAtual)
                    output << "\t" <<nodAtual << " -- " << auxG << "[color=lightgrey]"<< endl;
            }
            else if (auxT != -1) {
                if(auxT > nodAtual)
                    output << "\t" <<nodAtual << " -- " << auxT << endl;
                
            }
        }
    }

    nodAtual = 0;
    for (int i = 0; i < igraph_vcount(&T); i++) {
        nodAtual = i + 1;
        if(degree(T, i) > d){
            output << "\t" << nodAtual << " [color=""lightblue"", style=""filled""];" << "\n";
        }
    }
    output << "}" << endl;
    igraph_adjlist_destroy(&adjlistT);
    igraph_adjlist_destroy(&adjlistG);

}

void Heuristic::print_graphviz_neighbor(igraph_t &G, igraph_t &T1, int e_1, int e_2, int v, int u, const std::string &path, string &step, int & improvement){
    std::filesystem::create_directories("graphviz/" + path);
    std::string file_path = "graphviz/" + path + "/" + step + "-" + to_string(improvement)+ ".dot";
    std::ofstream print(file_path);                

    igraph_adjlist_t adjlistT;
    igraph_adjlist_init(&T1, &adjlistT, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVertT;

    igraph_adjlist_t adjlistG;
    igraph_adjlist_init(&G, &adjlistG, IGRAPH_OUT, IGRAPH_NO_LOOPS, IGRAPH_MULTIPLE);
    igraph_vector_int_t *adjVertG;

    print << "graph G {" << endl;

    int nodAtual = 0;
    for (int i = 0; i < igraph_vcount(&T1); i++) {
        nodAtual = i + 1;
        adjVertT = igraph_adjlist_get(&adjlistT, i);
        adjVertG = igraph_adjlist_get(&adjlistG, i);

        for (int j = 0; j < igraph_vector_int_size(adjVertG); j++) {
            int auxG = igraph_vector_int_e(adjVertG, j) + 1;
            int auxT = -1;
                for (int h = 0; h < igraph_vector_int_size(adjVertT); h++){
                    int auxT_aux = igraph_vector_int_e(adjVertT, h) + 1;
                    if(auxG == auxT_aux){
                        auxT = auxT_aux;
                        break;
                    }
                }
            if(auxT == -1){
                if(auxG > nodAtual){
                    if((nodAtual == e_1+1 && auxG == e_2+1) || (nodAtual == e_2+1 && auxG == e_1+1))
                        print << "\t" <<nodAtual << " -- " << auxG << "[color=green]" << "\n";
                    else
                        print << "\t" <<nodAtual << " -- " << auxG << "[color=lightgrey]"<< endl;
                }
            }
            else if (auxT != -1) {
                if(auxT > nodAtual){
                    if((nodAtual == u + 1 && auxT == v + 1) || (nodAtual == v + 1 && auxT == u + 1))
                        print << "\t" <<nodAtual << " -- " << auxT << "[color=red]" << "\n";
                    else
                        print << "\t" <<nodAtual << " -- " << auxT << endl;
                }
            }
        }
    }

    removeEdge(T1, u,v);
    addEdge(T1, e_1, e_2);
    nodAtual = 0;
    for (int j = 0; j < igraph_vcount(&T1); j++) {
        nodAtual = j + 1;
        if(degree(T1, j) > d){
            print << "\t" << nodAtual << " [color=""lightblue"", style=""filled""];" << "\n";
        }
    }
    print << "}" << endl;
    igraph_adjlist_destroy(&adjlistT);
    igraph_adjlist_destroy(&adjlistG);
    print.close();
}

void Heuristic::saveGraphvizFile(const std::string &path, string &step, igraph_t &g) {
    
    std::filesystem::create_directories("graphviz/" + path);
    std::string file_path = "graphviz/" + path + "/" + step + ".dot";
    std::ofstream print(file_path);
    
    if (!print.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << file_path << std::endl;
        return;
    }
    print_graphviz(print, g);
    print.close();
}





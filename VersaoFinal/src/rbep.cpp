#include "rbep.hpp"
#include <algorithm>
#include <iostream>

#include "roleta.hpp"

RBEP::RBEP(const Graph& pG):
    G(pG), T(pG.n,pG.m), InBT(G.n, 0), InPontas(G.n, false)//, RemPontas(G.n, false)
{
    G.name = "G";
    T.name = "T";
}

RBEP::~RBEP(){}


Graph RBEP::get_graph() const
{
    return T;
}

int RBEP::return_size_dbranch()
{
    return BT.size();
}

void RBEP::apply(std::mt19937_64& gen)
{

    BT.clear();

    for (int i = 0; i < G.n; i++)
    {
        InBT[i] = 0;
        InPontas[i] = false;
    }

    // cout << "DFS" << endl;
    G.detect_articulations_bridges();

    for(int v : G.articulation)
    {   
        T.add_vertice(v); 
        BT.push_back(v); 
        InBT[v] = 1; 
    }

    //Tratar Pontes:
    for(pair<int,int> vu : G.bridges)
    {
        int v = vu.first;
        int u = vu.second;

        T.add_vertice(v);
        T.add_vertice(u);

        T.add_edge(v,u);

        if (T.deg[v] > 2 && InBT[v] == 0)
        {
            BT.push_back(v);
            InBT[v] = 1;
        }

        if (T.deg[u] > 2 and InBT[u] == 0)
        {
            BT.push_back(u);
            InBT[u] = 1;
        }
    }
    // cout << "PONTES" << endl;

    // Expanção dos Vértices BT, como eles já são obrigatóriamente vértices dbranch
    // acrescenta todas as conexões
    for(int v : BT)
    {   
        for(int u : G.listAdj[v])
        {   
            if(T.find_vertice(u) == false)
            {   
                T.add_vertice(u);
                T.add_edge(v,u);
            }
            else
            {
                if ((T.deg[u] == 1 || InBT[u] == 1) && T.check_connection(v) != T.check_connection(u))
                {   
                    T.add_edge(v,u);
                }
            }
        }
    }
    // cout << "EXPANSAO" << endl;


    // Preenchendo o vetor de pontas
    for(int v : T.V)
    {
        if(T.deg[v] == 1 && G.deg[v] > 1)
        {
            InPontas[v] = true;
            Pontas.insert(v);
        }
        
    }

    //Se nenhuma ponta foi identificada, cria uma nova:
    if(Pontas.empty())
    {   

        int v = G.v_min_pagerank();
        int u = G.u_min_pagerank(v);

        T.add_vertice(v);
        T.add_vertice(u);
        T.add_edge(v,u);

        InPontas[v] = true;
        Pontas.insert(v);

        InPontas[u] = true;
        Pontas.insert(u);
        
    }
    // cout << "PONTAS" << endl;

    while(T.edge.size() < G.n - 1)
    {   

        if(Pontas.empty() == false)
        {   
            // cout << "EXISTE PONTAS" << endl;

            int v;
            int grauMax = 0;

            Roleta R = Roleta();

            for(int i : Pontas)
            {
                if(G.deg[i] > grauMax)
                {
                    grauMax = G.deg[i];
                }
            }
            grauMax = grauMax + 1;

            for(int p : Pontas)
            {
                double peso = grauMax - G.deg[p];
                R.Adicionar(p, peso);
            }

            v = R.Sortear(gen);

            Pontas.erase(v);
            InPontas[v] = false;


            vector<pair<int,pair<int,int>>> Nv;

            for(int u : G.listAdj[v])
                if(T.find_vertice(u) == false || (T.is_conection(u) != T.is_conection(v) && 
                (InBT[u] == 1 || T.deg[u] == 1))){
                    Nv.push_back(make_pair(u, make_pair(G.deg[u], T.deg[u])));
                }

            if(Nv.empty() == false)
            {
                int du_G;// = G.n + 1;
                int du_T;// = G.n + 1;
                int u;
                int grauMax2 = 0;

                Roleta R = Roleta();

                for(pair<int,pair<int,int>> j : Nv)
                {
                    du_G = j.second.first;
                    if(du_G > grauMax2)
                    {
                        grauMax2 = du_G;
                    }
                }
                grauMax2 = grauMax2 + 1;

                for(pair<int,pair<int,int>> i : Nv)
                {
                    u = i.first;
                    du_G = i.second.first;
                    du_T = i.second.second;

                    if(du_T == 0)
                    {
                        du_G = du_G - 1;
                    }
                    double peso = grauMax2 - du_G;
                    R.Adicionar(u, peso);

                }
                u = R.Sortear(gen);
                
                T.add_vertice(u);
                T.add_edge(v,u);
                
				if (T.deg[u] == 1)
                {
                    InPontas[u] = true;
                    Pontas.insert(u);
                }
				else
                {
                    if(InPontas[u] == true)
                    {
                        Pontas.erase(u);
                        InPontas[u] = false;
                    }
                }
            }

        }
        else
        {   
            // cout << "NAO EXISTE PONTAS" << endl;

            //Não existe uma nova ponta para ser explorada: necessário converter um vértice da árvore em ramificação.
            int v;
			int nCC = -1;
			vector<int> C;

            for (int i : T.V) // Todos os vértices da arvore atual.
            {   
                vector<int> listaC;
                set<int> listaCC;

                for (int u : G.listAdj[i]) 
                {

                    if (T.find_vertice(u) == false || 
                        (T.is_conection(u) != T.is_conection(i) && 
                        (InBT[u] == 1 || T.deg[u] == 1)))
                    {
                        listaC.push_back(u);
                        listaCC.insert(T.is_conection(u));
                    }
                }

                //Salvar o vértice com a maior quantidade de opções.
                if ((int)listaCC.size() > nCC)
                {
                    v = i;
                    nCC = listaCC.size();
                    C = listaC;
                }
            }

            if(C.size() > 0) //Se existir vértice que tenha opções que possa ser escolhida
            {   
                // cout << "ENCONTRADO PONTA" << endl;
                
                //Trasformar v em ramificação permite estabelecer novas conexões:
                BT.push_back(v);
                InBT[v] = 1;
                for(int u : C)
                {
                    if(T.is_conection(u) != T.is_conection(v)) //Expansão desse novo vértice
                    {
                        T.add_vertice(u);
                        T.add_edge(v,u);

						if(T.deg[u] == 1)
                        {
                            InPontas[u] = true;
                            Pontas.insert(u); //Atualização de Pontas
						}
                    }
                }

            }
            else
            {   
                // cout << "FORCAR BT" << endl;

                int v;
                int nCC = -1;
                vector<int> C;

                for (int i : T.V) // Todos os vértices de V
                {
                    vector<int> listaC;
                    set<int> listaCC;

                    for (int u : G.listAdj[i]) //Adjacentes de V no grafo.
                    {
                        if (T.is_conection(u) != T.is_conection(i)) // Ainda não estão conectados
                        {
                            listaC.push_back(u);
                            listaCC.insert(T.is_conection(u));
                        }
                    }

                    if ((int)listaCC.size() > nCC) //Seleciona o maior
                    {
                        v = i;
                        nCC = listaCC.size();
                        C = listaC;
                    }
                }
                //Torna esse novo vetor uma ramificação.
                BT.push_back(v);
                InBT[v] = 1;
            }
            
        }
    }

}


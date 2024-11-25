    #ifndef READFILE_H
    #define READFILE_H

    void readMediumInstances(char *ruta, Graph& G)
    {
        int n, m, src, dest, weight;
        char trash[128];

        ifstream f;
        f.open(ruta);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo";
            exit(1);
        }


        f >> n;
        f >> m;
        f >> trash;

        G.set_number_vert(n);

        for(int i = 0; i < m; i++)
            {
                f >> src;
                f >> dest;
                f >> weight;
                G.addEdge2(src-1,dest-1);
            }

        f.close();
    }


    void readSteind(char *ruta, Graph& G)
    {
        int n, m, src, dest, weight;


        ifstream f;
        f.open(ruta);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo";
            exit(1);
        }


        f >> n;
        f >> m;
        G.set_number_vert(n);

        for(int i = 0; i < m; i++)
            {
                f >> src;
                f >> dest;
                f >> weight;
                G.addEdge2(src-1,dest-1);
            }

        f.close();
    }


    void readLeighton(char *path, Graph& G)
    {
        int n, m, src, dest;
        char e, trash[128];


        ifstream f;
        f.open(path);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo"<<path<<endl;
            exit(1);
        }

        for(int i = 0;i < 33;i++)
                f.getline(trash,128);


        f>>trash;f>>trash;

        f >> n;
        f >> m;
        G.set_number_vert(n);

        for(int i = 0; i < m; i++)
            {
                f >> e;
                f >> src;
                f >> dest;
                G.addEdge2(src-1,dest-1);
            }

        f.close();
    }


    void readTSPLIB95(char *path, Graph& G)
    {
        int n, src, dest;
        char trash[128];

        ifstream f;
        f.open(path);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo";
            exit(1);
        }

        for(int i = 0;i < 3;i++)
            f.getline(trash,128);
        f>>trash;
        f>>trash;

        f >> n;

        for(int i = 0;i < 3;i++)
            f.getline(trash,128);


        G.set_number_vert(n);

        while(true)
            {
                f >> src;
                if(src == -1)
                    break;
                f >> dest;
                G.addEdge2(src-1,dest-1);
            }

        f.close();
    }

     void readKlingman(char *path, Graph &G)
    {
        int n, m, src, dest;
        char e, trash[128];


        ifstream f;
        f.open(path);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo";
            exit(1);
        }

        for(int i = 0;i < 22;i++)
                f.getline(trash,128);


        f>>trash;f>>trash;

        f >> n;
        f >> m;

        G.set_number_vert(n);

        f>>trash;
        for(int i = 0;i < n;i++)
                f.getline(trash,128);

        for(int i = 0; i < m; i++)
            {
                f >> e;
                f >> src;
                f >> dest;
                G.addEdge2(src-1,dest-1);
                f.getline(trash,128);
            }

        f.close();

    }


    void readGraphChen(char *ruta, Graph& G)
    {
        int n = 0, edge;
        char trash[128];

        ifstream f;
        f.open(ruta);
        if(f.fail())
        {
            cerr << "problema ao abrir o arquivo";
            exit(1);
        }


        f.get(trash,8);
        //if(strcmp(trash,"nNodes=")==0)
        //    cout<<"OK\n";

        f>>n;
        G.set_number_vert(n);

         f.getline(trash,128);
         f.getline(trash,128);
         f.getline(trash,128);

        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                f>>edge;
                //if(i == 99 && j == 99)
                //    cout<<edge<<endl;
                if(edge == 0 && i<j)
                    G.addEdge(i,j);
            }
            f.getline(trash,128);
        }

        f.close();
    }


    /*void readInstance(int inst,char *path, Graph &G )
    {
        switch(inst)
        {
            case 0: readMediumInstances(path, G);  break;
            case 1: readLeighton(path, G);  break;
            case 2: readKlingman(path, G);  break;
            case 3: readSteind(path, G);    break;
            case 4: readTSPLIB95(path, G);  break;

        }

    }*/

    void readInstance(int inst,char *path, Graph &G )
    {
        switch(inst)
        {
            case 0: readGraphChen(path,G); break;
            case 1: readGraphChen(path,G); break;
            case 2: readMediumInstances(path, G);  break;
            case 3: readMediumInstances(path, G);  break;
            case 4: readLeighton(path, G);  break;
            case 5: readSteind(path, G);    break;
            case 6: readTSPLIB95(path, G);  break;
        }

    }



    #endif // READFILE_H


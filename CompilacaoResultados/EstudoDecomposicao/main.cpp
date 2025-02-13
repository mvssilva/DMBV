#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <filesystem>

#include "include/dados.h"
#include "include/read.h"

int qtdArquivos = 3;

int main(int argc, char ** argv){

    Dados * d = dados_create(qtdArquivos);

    setup_ils(d, argv[1]);
    cout << "File 1 save!" << endl;

    setup_ils(d, argv[2]);
    cout << "File 2 save!" << endl;

    setup_ils(d, argv[3]);
    cout << "File 2 save!" << endl;

    // setup_ils(d, argv[4]);
    // cout << "File 3 save!" << endl;


    dados_sort(d);
    cout << "Sort complete!" << endl;

    /*
        Analises de resultados de vertices.
    */
    const char * dbranch = "const-dec";
    write_db_mediun(d, dbranch);
    write_db_large(d, dbranch);
    
    // write_gap_mediun(d, dbranch);
    // cout << "Write mediun complete!" << endl;
    // write_gap_large(d, dbranch);
    // cout << "Write large complete!" << endl;

    /*
        Analises de resultados de tempos dos algoritmos.
    */
    // const char * time = "search-time";
    // write_time_mediun(d, time);
    // cout << "Write mediun of time complete!" << endl;
    // write_time_large(d, time);
    // cout << "Write large of time complete!" << endl;

    /*
    Analises gerais
    */
    // const char * geral = "ils-variacoes-geral";
    // write_geral(d, geral);
    // write_geral_exato(d, geral);

    // cout << "Write geral complete!" << endl;

    // return 0;
}
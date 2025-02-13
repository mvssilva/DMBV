#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <filesystem>

#include "include/dados.hpp"
#include "include/read.hpp"

int qtdArquivos = 1;

int main(int argc, char ** argv){

    Dados * d = dados_create(qtdArquivos);

    setup(d, argv[1]);
    cout << "File 1 save!" << endl;

    dados_sort(d);
    cout << "Sort complete!" << endl;

    const char * name = "alfa";
    write_customize(d, name);
    return 0;
}
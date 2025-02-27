
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set>
#include <sstream> 
#include <string>
#include <cmath> 
using namespace std;

#define d 2

int main(int argc, char *argv[]){
    
    int n, m, db, weight;
    string name;

    ifstream f(argv[1]);

    string line;
    while (getline(f, line)) {
        istringstream ss(line);
        string token;

        getline(ss, name, ';');

        getline(ss, token, ';');
        n = stoi(token); 

        getline(ss, token, ';');
        m = stoi(token); 

        getline(ss, token, ';');
        db = stoi(token); 

        getline(ss, token, ';');
        weight = stoi(token); 

        double density = (2.0 * m) / (n * (n - 1)); 
        double bestAlfa = 0; 
        double minDifference = INFINITY; 
        
        for (double alfa = 1.0; alfa <= 10.0; alfa += 0.001) { 
            double aux = 1.0 / (alfa * density); 
            double difference = abs(aux - db); 
            cout << alfa << ";" << name << endl;
            if (difference < minDifference) {
                minDifference = difference; 
                bestAlfa = alfa; 
            }
        }

        ofstream result("output/rel_densidade_alfa.csv", std::ios_base::app);
        result << name << ";" << n << ";" << m << ";" << density << ";" << bestAlfa << ";" << endl;
        result.close();
    }

    f.close();



    
}
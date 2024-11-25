#include "Roleta.h"
#include <iostream>

ItemRoleta::ItemRoleta(int k, double p)
{
	key = k;
	prob = p;
	lowerBound = 0;
	upperBound = 0;
	probCalc = 0;
}

Roleta::Roleta()
{
	soma = 0;
}

Roleta::~Roleta()
{
}

void Roleta::Adicionar(int key, double probabilidade)
{
	if (probabilidade < 0)
		itens.push_back(ItemRoleta(key, 0));
	else
		itens.push_back(ItemRoleta(key, probabilidade));
}

int Roleta::Sortear(std::mt19937_64& gen)
{
	double soma = 0;
	for (int i = 0; i < (int)itens.size(); i++)
		soma = soma + itens[i].prob;

	/*ORIGINAL*/
	// uniform_real_distribution<> rndInterval(0, 1);
	// random_device rd;
	// mt19937_64 genOrigen(rd());
	// double d = rndInterval(genOrigen);
    
	/*Seed Fixa*/
	uniform_real_distribution<> rndInterval(0, 1);
	double d = rndInterval(gen);

	double step = 0;
	for (int i = 0; i < (int)itens.size(); i++)
	{
		itens[i].probCalc = itens[i].prob / soma;
		itens[i].lowerBound = step;
		itens[i].upperBound = step + itens[i].probCalc;
		step = step + itens[i].probCalc;

		if (d >= itens[i].lowerBound && d < itens[i].upperBound)
		{
			return itens[i].key;
		}
	}
	return itens[itens.size() - 1].key;
}

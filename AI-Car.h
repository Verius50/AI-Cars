#include <iostream>
#pragma once
#define lenght 5
#define layersNumber 1
#define layersSize 10
#define weight 5

class AICar
{
	double weights[weight][lenght];

public:
	void InitRand();
	int InputData(double data[lenght]);

	//void print();
	void SaveWeights();
	void InitBestWeights(std::string path);
};


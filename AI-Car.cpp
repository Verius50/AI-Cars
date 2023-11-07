#include "AI-Car.h"
#include <fstream>
#define weight 5
#define lenght 5

void AICar::InitRand()
{
	for (int i = 0; i < weight; i++)
		for (int j = 0; j < lenght; j++)
			weights[i][j] = rand() % 100;
}

void AICar::SaveWeights()
{
	std::ofstream file("weights.txt");
	for (int i = 0; i < weight; i++)
		for (int j = 0; j < lenght; j++)
			file << weights[i][j] << ' ';
	file.close();
}

void AICar::InitBestWeights(std::string path)
{
	int tmp = 0;
	std::ifstream file(path);
	for (int i = 0; i < weight; i++)
		for (int j = 0; j < lenght; j++)
		{
			file >> tmp;
			if (path == "weights.txt")
				weights[i][j] = tmp + rand() % 40 - rand() % 40;
			else weights[i][j] = tmp + rand() % 2 - rand() % 2;
		}
	file.close();
}

int AICar::InputData(double data[lenght])
{
	double answ[lenght] = { 0.0 , 0.0 }, max = 0;
	int nm = 0;
	for (int i = 0; i < weight; i++)
		for (int j = 0; j < lenght; j++)
			answ[i] += data[j] * weights[i][j];
	for (int j = 0; j < lenght; j++)
		if (answ[j] > max)
		{
			max = answ[j];
			nm = j;
		}
	return nm;
}

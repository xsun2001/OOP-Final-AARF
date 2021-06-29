#pragma once
#include <random>
class RandomReal
{
public:
	RandomReal( double randomMin, double randomMax ) : rd(), gen( rd() ), dis( randomMin, randomMax ) {}
	double operator()()
	{
		return dis( gen );
	}

private:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
};
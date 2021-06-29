#pragma once
#include <random>

class RandomUniformReal
{
public:
	RandomUniformReal( double randomMin, double randomMax ) : rd(), gen( rd() ), dis( randomMin, randomMax ) {}
	double operator()()
	{
		return dis( gen );
	}

private:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
};

class RandomNormalReal
{
public:
	RandomNormalReal( double randomMin, double randomMax ) : min( randomMin ), max( randomMax ), radius( ( max - min ) / 6 ), center( ( max + min ) / 2 ), rd(), gen( rd() ), dis() {}
	double operator()()
	{
		double x;
		do {
			x = center + dis( gen ) * radius;
		} while ( x < min || x > max );
		return x;
	}

private:
	double min, max, radius, center;
	std::random_device rd;
	std::mt19937 gen;
	std::normal_distribution<> dis;
};
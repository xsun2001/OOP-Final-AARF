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

// Multi-dimensional array
template<typename T, size_t size, size_t... sizeOthers>
class MDArray
{
	MDArray<T, sizeOthers...> data[size];

public:
	MDArray<T, sizeOthers...> &operator[]( size_t idx )
	{
		return data[idx];
	}
};

template<typename T, size_t size>
class MDArray<T, size>
{
	T data[size];

public:
	T &operator[]( size_t idx )
	{
		return data[idx];
	}
};

template<typename T>
class DynamicMDArray
{
	T *data;
	size_t tsize, dim, *dsize;

public:
	explicit DynamicMDArray( std::initializer_list<size_t> size )
	{
		dim = size.size();
		dsize = new size_t[dim];
		std::copy( size.begin(), size.end(), dsize );
		tsize = 1;
		for ( int i = dim - 1; i >= 0; i-- ) {
			size_t ss = tsize * dsize[i];
			dsize[i] = tsize;
			tsize = ss;
		}
		data = new T[tsize];
	}
	~DynamicMDArray()
	{
		delete[] data;
		delete[] dsize;
	}
	T &at( std::initializer_list<size_t> pos )
	{
		assert( dim == pos.size() );
		size_t idx = 0, i = 0;
		for ( unsigned long po : pos ) {
			idx += po * dsize[i++];
		}
		return data[idx];
	}
};
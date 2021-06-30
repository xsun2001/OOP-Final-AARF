#pragma once
#include <QPainter>
#include <tuple>
#include <vector>

using Point = std::tuple<double, double>;
using TwoPoints = std::tuple<Point, Point>;

class Graph
{
public:
	void paint( QPainter *painter );
	Graph( double width, double height, std::vector<TwoPoints> obstacles, std::vector<TwoPoints> nets );
	Graph( const Graph &other ) = default;
	void setCdtEdges( std::vector<TwoPoints> cdt_edges );
	void setRoute( int netId, std::vector<Point> route );
	double getWidth() const;
	double getHeight() const;
	const std::vector<TwoPoints> &getObstacles() const;
	const std::vector<TwoPoints> &getNets() const;
	const std::vector<TwoPoints> &getCdtEdges() const;
	const std::vector<std::vector<Point>> &getRoutes() const;

private:
	double width, height;
	std::vector<TwoPoints> obstacles, nets, cdt_edges;
	std::vector<std::vector<Point>> routes;
};

Graph *generateRandomGraph( double width, double height, int obsCount, int netCount );
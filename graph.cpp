#include "graph.h"
#include "util.h"
#include <QPainterPath>
#include <utility>
void Graph::paint( QPainter *painter )
{
	painter->setPen( QPen( Qt::black, 1 ) );
	painter->setBrush( Qt::white );
	painter->drawRect( 0, 0, width, height );
	painter->setBrush( Qt::black );
	for ( auto [pa, pb] : obstacles ) {
		auto [ax, ay] = pa;
		auto [bx, by] = pb;
		painter->fillRect( ax, ay, bx - ax, by - ay, Qt::black );
	}
	for ( auto [pa, pb] : nets ) {
		auto [ax, ay] = pa;
		auto [bx, by] = pb;
		const double radius = 1;
		painter->drawEllipse( { ax, ay }, radius, radius );
		painter->drawEllipse( { bx, by }, radius, radius );
	}
	painter->setPen( QPen( Qt::gray, 2 ) );
	for ( auto [pa, pb] : cdt_edges ) {
		auto [ax, ay] = pa;
		auto [bx, by] = pb;
		painter->drawLine( ax, ay, bx, by );
	}
	painter->setPen( QPen( Qt::red, 4 ) );
	for ( auto &route : routes ) {
		QPainterPath path;
		bool first = true;
		for ( auto [x, y] : route ) {
			first ? path.moveTo( x, y ) : path.lineTo( x, y );
			first = false;
		}
		painter->drawPath( path );
	}
}
Graph::Graph( double width, double height, std::vector<TwoPoints> obstacles, std::vector<TwoPoints> nets )
	: width( width ), height( height ), obstacles( std::move( obstacles ) ), nets( std::move( nets ) ), cdt_edges(), routes( nets.size() )
{
#define CHECK_POINT( P )                        \
	auto [P##_x, P##_y] = P;                    \
	assert( ( P##_x >= 0 && P##_x <= width ) ); \
	assert( ( P##_y >= 0 && P##_y <= width ) )
#define CHECK_REC( PA, PB )    \
	CHECK_POINT( PA );         \
	CHECK_POINT( PB );         \
	assert( PB##_x > PA##_x ); \
	assert( PB##_y > PA##_y )

	assert( width > 0 );
	assert( height > 0 );
	for ( auto [pa, pb] : obstacles ) {
		CHECK_REC( pa, pb );
	}
	for ( auto [pa, pb] : nets ) {
		CHECK_POINT( pa );
		CHECK_POINT( pb );
	}

#undef CHECK_POINT
#undef CHECK_REC
}
void Graph::setCdtEdges( std::vector<TwoPoints> cdt_edges )
{
	this->cdt_edges = std::move( cdt_edges );
}
void Graph::setRoute( int netId, std::vector<Point> route )
{
	assert( ( netId >= 0 && netId < nets.size() ) );
	routes[netId] = std::move( route );
}
double Graph::getWidth() const
{
	return width;
}
double Graph::getHeight() const
{
	return height;
}
const std::vector<Graph::TwoPoints> &Graph::getObstacles() const
{
	return obstacles;
}
const std::vector<Graph::TwoPoints> &Graph::getNets() const
{
	return nets;
}
const std::vector<Graph::TwoPoints> &Graph::getCdtEdges() const
{
	return cdt_edges;
}
const std::vector<std::vector<Graph::Point>> &Graph::getRoutes() const
{
	return routes;
}

inline bool inRectangle( Graph::TwoPoints &rec, Graph::Point &point )
{
	auto [p1, p2] = rec;
	auto [x, y] = point;
	auto [x1, y1] = p1;
	auto [x2, y2] = p2;
	return x > x1 && x < x2 && y > y1 && y < y2;
}

Graph *generateRandomGraph( double width, double height, int obsCount, int netCount )
{
	RandomReal random( 0, 1 );
	// Plain O(N^2) Algorithm
	std::vector<Graph::TwoPoints> obstacles;
	for ( int i = 0; i < obsCount; i++ ) {
		double x = random() * width, y = random() * height, w = random() * ( width - x ), h = random() * ( height - y );
		obstacles.push_back( { { x, y }, { x + w, y + h } } );
	}
	std::vector<Graph::TwoPoints> nets;
	for ( int i = 0; i < netCount; i++ ) {
		bool pushed = false;
		while ( !pushed ) {
			Graph::Point p1( random() * width, random() * height ), p2( random() * width, random() * height );
			for ( auto &rec : obstacles ) {
				if ( !inRectangle( rec, p1 ) && !inRectangle( rec, p2 ) ) {
					nets.emplace_back( p1, p2 );
					pushed = true;
				}
			}
		}
	}
	return new Graph( width, height, obstacles, nets );
}

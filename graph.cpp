#include "graph.h"
#include "util.h"
#include <QPainterPath>
#include <cmath>
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

inline bool isIntersect( Graph::TwoPoints &rec1, Graph::TwoPoints &rec2 )
{
#define X( P ) ( std::get<0>( P ) )
#define Y( P ) ( std::get<1>( P ) )
	auto [p11, p12] = rec1;
	auto [p21, p22] = rec2;
	return X( p21 ) < X( p12 ) && Y( p21 ) < Y( p12 ) && X( p22 ) > X( p11 ) && Y( p22 ) > Y( p11 );
#undef X
#undef Y
}

Graph *generateRandomGraph( double width, double height, int obsCount, int netCount )
{
	RandomUniformReal rd1( 0, 1 );
	RandomNormalReal rd2( 0, 1 );
	// Plain O(N^2) Algorithm
	std::vector<Graph::TwoPoints> obstacles;
	double areaFactor = 1.0 / sqrt( 2 * obsCount );
	for ( int i = 0; i < obsCount; i++ ) {
		bool pushed = false;
		while ( !pushed ) {
			pushed = true;
			double x = rd1() * width, y = rd1() * height, w = std::min( rd2() * width * areaFactor, width - x ), h = std::min( rd2() * height * areaFactor, height - y );
			Graph::TwoPoints rec( { x, y }, { x + w, y + h } );
			for ( auto &other : obstacles ) {
				if ( isIntersect( rec, other ) ) {
					pushed = false;
					break;
				}
			}
			if ( pushed ) {
				obstacles.push_back( rec );
			}
		}
	}
	std::vector<Graph::TwoPoints> nets;
	for ( int i = 0; i < netCount; i++ ) {
		bool pushed = false;
		while ( !pushed ) {
			pushed = true;
			Graph::Point p1( rd1() * width, rd1() * height ), p2( rd1() * width, rd1() * height );
			for ( auto &rec : obstacles ) {
				if ( inRectangle( rec, p1 ) || inRectangle( rec, p2 ) ) {
					pushed = false;
					break;
				}
			}
			if ( pushed ) {
				nets.emplace_back( p1, p2 );
			}
		}
	}
	return new Graph( width, height, obstacles, nets );
}

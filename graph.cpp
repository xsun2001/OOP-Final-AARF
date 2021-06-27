#include "graph.h"
#include <QPainterPath>
#include <utility>
void Graph::paint( QPainter *painter )
{
	painter->setPen( QPen( Qt::black, 3 ) );
	painter->setBrush( Qt::black );
	painter->fillRect( 0, 0, width, height, Qt::white );
	painter->drawRect( 0, 0, width, height );
	for ( auto [pa, pb] : obstacles ) {
		auto [ax, ay] = pa;
		auto [bx, by] = pb;
		painter->fillRect( ax, ay, bx - ax, by - ay, Qt::black );
	}
	for ( auto [pa, pb] : nets ) {
		auto [ax, ay] = pa;
		auto [bx, by] = pb;
		const double diameter = 2;
		painter->drawEllipse( ax - diameter / 2, ay - diameter / 2, diameter, diameter );
		painter->drawEllipse( bx - diameter / 2, by - diameter / 2, diameter, diameter );
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

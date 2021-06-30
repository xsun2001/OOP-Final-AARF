#include "algo.h"
#include "util.h"
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <vector>

// CDT Algorithm

struct CDTGraph {
	explicit CDTGraph( Graph *graph ) : nodes(), constrained_edges()
	{
		double w = graph->getWidth(), h = graph->getHeight();
		addRectangle( { 0, 0 }, { w, h } );
		for ( auto [p1, p2] : graph->getObstacles() ) {
			addRectangle( p1, p2 );
		}
		for ( auto [p1, p2] : graph->getNets() ) {
			nodes.emplace_back( std::get<0>( p1 ), std::get<1>( p1 ) );
			nodes.emplace_back( std::get<0>( p2 ), std::get<1>( p2 ) );
		}
	}
	void addRectangle( Point p1, Point p2 )
	{
		auto [x1, y1] = p1;
		auto [x2, y2] = p2;
		auto node_idx = nodes.size();
		nodes.emplace_back( x1, y1 );
		nodes.emplace_back( x2, y1 );
		nodes.emplace_back( x1, y2 );
		nodes.emplace_back( x2, y2 );
		constrained_edges.emplace_back( node_idx, node_idx + 1 );
		constrained_edges.emplace_back( node_idx + 1, node_idx + 1 );
		constrained_edges.emplace_back( node_idx + 2, node_idx + 1 );
		constrained_edges.emplace_back( node_idx + 3, node_idx );
	}
	std::vector<std::tuple<double, double, int>> nodes;
	std::vector<std::tuple<int, int>> constrained_edges;
};

struct CDTHelper {
	explicit CDTHelper( Graph *graph ) : graph( graph ), cdt_graph( graph ), vertices{ 2 + 2 * cdt_graph.nodes.size(), 3 }, adjTriangles{ 2 + 2 * cdt_graph.nodes.size(), 3 } {}

	// Construct constrained delaunay triangulations
	void construct()
	{
		constructDT();
		extractCDTEdges();
	}
	void extractCDTEdges()
	{
		std::vector<std::set<unsigned>> edges( cdt_graph.nodes.size() );
		std::vector<bool> vis( triangle_count, false );
		std::queue<unsigned> triQueue;
		auto tryAddEdge = [&]( unsigned x, unsigned y ) {
			unsigned a = std::min( x, y ), b = std::max( x, y );
			if ( b >= cdt_graph.nodes.size() - 3 ) {
				return;
			}
			edges[a].insert( b );
		};
		auto tryPushTriangle = [&]( unsigned t ) {
			if ( t == 0 || vis[t] ) {
				return;
			}
			triQueue.push( t );
			vis[t] = true;
		};
		tryPushTriangle( 1 );
		while ( !triQueue.empty() ) {
			auto t = triQueue.front();
			triQueue.pop();
			auto [p1, p2, p3] = triangleVertices( t );
			tryAddEdge( p1, p2 );
			tryAddEdge( p2, p3 );
			tryAddEdge( p3, p1 );
			tryPushTriangle( adjTriangles.at( { t, 0 } ) );
			tryPushTriangle( adjTriangles.at( { t, 1 } ) );
			tryPushTriangle( adjTriangles.at( { t, 2 } ) );
		}
		std::vector<TwoPoints> cdt_edges;
		for ( int i = 0; i < edges.size(); ++i ) {
			for ( const auto &j : edges[i] ) {
				auto [x1, y1, b1] = cdt_graph.nodes[i];
				auto [x2, y2, b2] = cdt_graph.nodes[j];
				cdt_edges.push_back( { { x1, y1 }, { x2, y2 } } );
			}
		}
		graph->setCdtEdges( cdt_edges );
	}

	// Construct delaunay triangulations
	// Algorithm framework
	void constructDT()
	{
		normalize();
		binSort();
		setupSuperTriangle();
		for ( int i = 0; i < cdt_graph.nodes.size() - 3; ++i ) {
			unsigned triangle = findEncloseTriangle( i );
			splitTriangle( triangle, i );
			testAndSwapTriangle( i );
			assert( checkTriangle() );
		}
		denormalize();
	}
	// Subroutines
	void normalize()
	{
		std::cout << "normalize" << std::endl;
		dmax = std::max( graph->getWidth(), graph->getHeight() );
		for ( auto &[x, y, _] : cdt_graph.nodes ) {
			x /= dmax;
			y /= dmax;
		}
	}
	void binSort()
	{
		std::cout << "bin sort" << std::endl;
		int ndiv = pow( cdt_graph.nodes.size(), 0.25 );
		double k = ndiv * 0.99;
		for ( auto &[x, y, bin] : cdt_graph.nodes ) {
			int i = y * k;
			int j = x * k;
			bin = i & 1 ? i * ndiv + j + 1 : ( i + 1 ) * ndiv - j;
		}
		std::sort( cdt_graph.nodes.begin(), cdt_graph.nodes.end(), []( auto &p1, auto &p2 ) { return std::get<2>( p1 ) < std::get<2>( p2 ); } );
		for ( const auto &node : cdt_graph.nodes ) {
			printNode( node );
			std::cout << std::endl;
		}
	}
	void setupSuperTriangle()
	{
		std::cout << "super triangle" << std::endl;
		unsigned node_idx = cdt_graph.nodes.size();
		cdt_graph.nodes.emplace_back( -100, -100 );
		cdt_graph.nodes.emplace_back( 100, -100 );
		cdt_graph.nodes.emplace_back( 0, 100 );
		pushTriangle( node_idx, node_idx + 1, node_idx + 2, 0, 0, 0 );
	}
	unsigned findEncloseTriangle( unsigned p )
	{
		std::cout << "finding enclosed triangle for ";
		printNode( cdt_graph.nodes[p] );
		std::cout << " : ";
		unsigned triangle = triangle_count;
		for ( ;; ) {
			if ( onRightHand( triangle, 0, 1, p ) ) {
				triangle = adjTriangles.at( { triangle, 0 } );
			} else if ( onRightHand( triangle, 1, 2, p ) ) {
				triangle = adjTriangles.at( { triangle, 1 } );
			} else if ( onRightHand( triangle, 2, 0, p ) ) {
				triangle = adjTriangles.at( { triangle, 2 } );
			} else {
				break;
			}
		}
		printTriangle( triangle );
		std::cout << std::endl;
		return triangle;
	}
	void splitTriangle( unsigned triangle, unsigned node )
	{
		auto [p0, p1, p2] = triangleVertices( triangle );
		auto nt1 = triangle_count + 1, nt2 = triangle_count + 2;
		auto &adj0 = adjTriangles.at( { triangle, 0 } ), &adj2 = adjTriangles.at( { triangle, 2 } );
		pushTriangle( p0, p1, node, adj0, triangle, nt2 );
		pushTriangle( p0, node, p2, nt1, triangle, adj2 );
		connectBack( nt1, triangle, adj0 );
		connectBack( nt2, triangle, adj2 );
		vertices.at( { triangle, 0 } ) = node;
		adj0 = nt1;
		adj2 = nt2;
		triangle_stack.push( triangle );
		triangle_stack.push( nt1 );
		triangle_stack.push( nt2 );
	}
	void testAndSwapTriangle( int p )
	{
		while ( !triangle_stack.empty() ) {
			auto tl = triangle_stack.top();
			triangle_stack.pop();

			auto [tl0, tl1, tl2] = triangleVertices( tl );
			unsigned tr, ta, tb, tc;
			unsigned ploc, tr2, tr2loc;
			if ( p == tl0 ) {
				ploc = 0, tr2 = tl1;
			} else if ( p == tl1 ) {
				ploc = 1, tr2 = tl2;
			} else /* if ( p == tl2 ) */ {
				ploc = 2, tr2 = tl0;
			}
			tr = adjTriangles.at( { tl, ( ploc + 1 ) % 3 } );
			tc = adjTriangles.at( { tl, ( ploc + 2 ) % 3 } );

			if ( tr == 0 ) {
				continue;
			}

			auto [p1, p2, p3] = triangleVertices( tr );
			if ( tr2 == p1 ) {
				tr2loc = 0;
			} else if ( tr2 == p2 ) {
				tr2loc = 1;
			} else /* if ( tr2 == p3 ) */ {
				tr2loc = 2;
			}
			ta = adjTriangles.at( { tr, tr2loc } );
			tb = adjTriangles.at( { tr, ( tr2loc + 1 ) % 3 } );

			auto [x1, y1, b1] = cdt_graph.nodes[p1];
			auto [x2, y2, b2] = cdt_graph.nodes[p2];
			auto [x3, y3, b3] = cdt_graph.nodes[p3];
			auto [xp, yp, bp] = cdt_graph.nodes[p];
			auto x13 = x1 - x3, y13 = y1 - y3, x23 = x2 - x3, y23 = y2 - y3, x1p = x1 - xp, y1p = y1 - yp, x2p = x2 - xp, y2p = y2 - yp;

			auto cosa = x13 * x23 + y13 * y23, cosb = x2p * x1p + y2p * y1p;
			if ( cosa >= 0 && cosb >= 0 ) {
				continue;
			} else if ( cosa < 0 && cosb < 0 ) {
				continue;
			} else {
				auto sina = x13 * y23 - x23 * y13, sinb = x2p * y1p - x1p * y2p;
				auto sinab = sina * cosb + sinb * cosa;
				if ( sinab >= 0 ) {
					continue;
				}
			}

			vertices.at( { tl, ( ploc + 2 ) % 3 } ) = vertices.at( { tr, ( tr2loc + 1 ) % 3 } );
			vertices.at( { tr, tr2loc } ) = p;
			connect( tl, tr, ta, ( ploc + 1 ) % 3 );
			adjTriangles.at( { tl, ( ploc + 2 ) % 3 } ) = tr;
			adjTriangles.at( { tr, tr2loc } ) = tl;
			adjTriangles.at( { tr, ( tr2loc + 1 ) % 3 } ) = tb;
			connect( tr, tl, tc, ( tr2loc + 2 ) % 3 );

			triangle_stack.push( tl );
			triangle_stack.push( tr );
		}
	}
	void denormalize()
	{
		for ( auto &[x, y, _] : cdt_graph.nodes ) {
			x *= dmax;
			y *= dmax;
		}
	}
	// Help functions
	bool onRightHand( unsigned triangle, unsigned t1, unsigned t2, unsigned p )
	{
		unsigned p1 = vertices.at( { triangle, t1 } );
		unsigned p2 = vertices.at( { triangle, t2 } );
		auto [x1, y1, b1] = cdt_graph.nodes[p1];
		auto [x2, y2, b2] = cdt_graph.nodes[p2];
		auto [x, y, b] = cdt_graph.nodes[p];
		return ( x2 - x1 ) * ( y - y1 ) - ( x - x1 ) * ( y2 - y1 ) < 0;
	}
	void pushTriangle( unsigned p0, unsigned p1, unsigned p2, unsigned adj0, unsigned adj1, unsigned adj2 )
	{
		auto idx = ++triangle_count;
		vertices.at( { idx, 0 } ) = p0;
		vertices.at( { idx, 1 } ) = p1;
		vertices.at( { idx, 2 } ) = p2;
		adjTriangles.at( { idx, 0 } ) = adj0;
		adjTriangles.at( { idx, 1 } ) = adj1;
		adjTriangles.at( { idx, 2 } ) = adj2;
	}
	std::tuple<unsigned, unsigned, unsigned> triangleVertices( unsigned t )
	{
		return { vertices.at( { t, 0 } ),
				 vertices.at( { t, 1 } ),
				 vertices.at( { t, 2 } ) };
	}
	void tryPushToStack( unsigned t )
	{
		if ( t > 0 ) {
			triangle_stack.push( t );
		}
	}
	void connect( unsigned triangle, unsigned old, unsigned other, unsigned idx )
	{
		adjTriangles.at( { triangle, idx } ) = other;
		connectBack( triangle, old, other );
	}
	void connectBack( unsigned triangle, unsigned old, unsigned other )
	{
		for ( unsigned i = 0; i < 3; i++ ) {
			auto &adj = adjTriangles.at( { other, i } );
			if ( adj == old ) {
				adj = triangle;
			}
		}
	}
	void printNode( const std::tuple<double, double, unsigned> &node )
	{
		std::cout << "(" << std::get<0>( node ) << ", " << std::get<1>( node ) << ", " << std::get<2>( node ) << ") ";
	}
	void printTriangle( unsigned t )
	{
		std::cout << t << " ";
		auto [p1, p2, p3] = triangleVertices( t );
		printNode( cdt_graph.nodes[p1] );
		printNode( cdt_graph.nodes[p2] );
		printNode( cdt_graph.nodes[p3] );
	}
	bool checkTriangle()
	{
		std::vector<bool> vis( triangle_count + 1, false );
		std::queue<unsigned> q;
		q.push( 1 );
		while ( !q.empty() ) {
			auto t = q.front();
			q.pop();
			if ( vis[t] ) {
				continue;
			}
			vis[t] = true;
			for ( unsigned i = 0; i < 3; i++ ) {
				auto t2 = adjTriangles.at( { t, i } );
				if ( t2 == 0 ) {
					continue;
				}
				bool ok = false;
				for ( unsigned j = 0; j < 3; j++ ) {
					if ( adjTriangles.at( { t2, j } ) == t ) {
						ok = true;
					}
				}
				if ( !ok ) {
					return false;
				}
				q.push( t2 );
			}
		}
		return true;
	}

	Graph *graph;
	CDTGraph cdt_graph;
	DynamicMDArray<unsigned> vertices, adjTriangles;
	std::stack<unsigned> triangle_stack;
	unsigned triangle_count = 0;
	double dmax = 0;
};

void constructCDT( Graph *graph )
{
	CDTHelper( graph ).construct();
}
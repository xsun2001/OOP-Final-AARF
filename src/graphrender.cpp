#include "graphrender.h"
#include <QPainter>

void GraphRender::onGraphChanged( Graph *newGraph )
{
	graph = newGraph;
	update();
}

void GraphRender::paintEvent( QPaintEvent *event )
{
	if ( graph == nullptr ) {
		return;
	}

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );

	double graphAR = graph->getWidth() / graph->getHeight();
	double widgetWidth = width(), widgetHeight = height(), widgetAR = widgetWidth / widgetHeight;
	double newWidth = graphAR > widgetAR ? widgetWidth : widgetHeight * graphAR;
	double newHeight = graphAR > widgetAR ? widgetWidth / graphAR : widgetHeight;
	painter.setViewport( ( widgetWidth - newWidth ) / 2, ( widgetHeight - newHeight ) / 2, newWidth, newHeight );
	painter.setWindow( 0, 0, graph->getWidth(), graph->getHeight() );

	graph->paint( &painter );
}

#include "graphrender.h"
#include <QPainter>

void GraphRender::onGraphChanged( Graph *newGraph )
{
	graph = newGraph;
	update();
}

void GraphRender::paintEvent( QPaintEvent *event )
{
	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );
	painter.drawText( 50, 50, "TEST" );
}

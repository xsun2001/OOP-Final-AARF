#pragma once
#include "graph.h"
#include <QWidget>

class GraphRender : public QWidget
{
	Q_OBJECT
public slots:
	void onGraphChanged( Graph * );

protected:
	void paintEvent( QPaintEvent *event ) override;

private:
	Graph *graph = nullptr;
};
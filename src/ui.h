#pragma once
#include "graphrender.h"
#include <QLineEdit>
#include <QMainWindow>

class MainUI : public QWidget
{
	Q_OBJECT
public:
	MainUI();
private slots:
	void generate();

private:
	// width, height, obstacle count, net count
	QLineEdit *inputs[4];
	GraphRender *graphRender;
};
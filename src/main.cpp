#include "ui.h"
#include <QApplication>

int main( int argc, char *argv[] )
{
	QApplication a( argc, argv );
	MainUI ui;
	ui.show();
	return QApplication::exec();
}

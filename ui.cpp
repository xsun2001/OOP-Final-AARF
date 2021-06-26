#include "ui.h"
#include "graphrender.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QValidator>
#include <array>

MainUI::MainUI()
{
	auto *mainLayout = new QHBoxLayout;

	auto *leftPanel = new QWidget;
	auto *leftLayout = new QVBoxLayout;

	auto *intValidator = new QIntValidator;
	auto *inputPanel = new QWidget;
	auto *inputLayout = new QGridLayout;
	std::array inputLabels = { "Label 1: " };
	for ( int i = 0; i < inputLabels.size(); ++i ) {
		inputLayout->addWidget( new QLabel{ inputLabels[i] }, i, 0, 1, 1 );
		auto *input = inputs[i] = new QLineEdit;
		input->setValidator( intValidator );
		inputLayout->addWidget( input, i, 1, 1, 2 );
	}
	inputPanel->setLayout( inputLayout );

	auto *confirmButton = new QPushButton{ "Confirm" };

	leftLayout->addWidget( inputPanel );
	leftLayout->addStretch();
	leftLayout->addWidget( confirmButton );
	leftPanel->setLayout( leftLayout );

	auto *graphRender = new GraphRender;
	graphRender->setFixedSize( 500, 500 );

	mainLayout->addWidget( leftPanel );
	mainLayout->addWidget( graphRender );
	setLayout( mainLayout );
}

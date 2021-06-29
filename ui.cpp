#include "ui.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <array>

MainUI::MainUI()
{
	auto *mainLayout = new QHBoxLayout;

	auto *leftPanel = new QWidget;
	auto *leftLayout = new QVBoxLayout;

	auto *intValidator = new QIntValidator;
	intValidator->setBottom( 0 );
	auto *inputPanel = new QWidget;
	auto *inputLayout = new QGridLayout;
	std::array inputLabels = { "Width: ", "Height: ", "Obs. count: ", "Net count:" };
	for ( int i = 0; i < inputLabels.size(); ++i ) {
		auto *fieldLabel = new QLabel{ inputLabels[i] };
		fieldLabel->setAlignment( Qt::AlignRight );
		inputLayout->addWidget( fieldLabel, i, 0, 1, 1 );
		auto *input = inputs[i] = new QLineEdit;
		input->setValidator( intValidator );
		inputLayout->addWidget( input, i, 1, 1, 2 );
	}
	inputPanel->setLayout( inputLayout );

	auto *confirmButton = new QPushButton{ "Generate" };
	connect( confirmButton, &QAbstractButton::clicked, this, &MainUI::generate );

	leftLayout->addWidget( inputPanel );
	leftLayout->addStretch();
	leftLayout->addWidget( confirmButton );
	leftPanel->setLayout( leftLayout );

	graphRender = new GraphRender;
	graphRender->setFixedSize( 500, 500 );

	mainLayout->addWidget( leftPanel );
	mainLayout->addWidget( graphRender );
	setLayout( mainLayout );
}

void MainUI::generate()
{
	int numbers[4];
	for ( int i = 0; i < 4; i++ ) {
		if ( ( numbers[i] = inputs[i]->text().toInt() ) == 0 ) {
			return;
		}
	}
	auto graph = generateRandomGraph( numbers[0], numbers[1], numbers[2], numbers[3] );
	graphRender->onGraphChanged( graph );
}

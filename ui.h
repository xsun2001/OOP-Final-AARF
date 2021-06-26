#include <QLineEdit>
#include <QMainWindow>

class MainUI : public QWidget
{
	Q_OBJECT
public:
	MainUI();

private:
	QLineEdit *inputs[1];
};
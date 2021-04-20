#include "compdir.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CompDir w;
	w.show();

	return a.exec();
}

#include "geomodeltest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GeoModelTest w;
	w.show();
	return a.exec();
}

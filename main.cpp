#include "vlcplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VLCPlayer w;
	w.show();
	return a.exec();
}

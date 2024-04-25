#include "MainWindow.hpp"

#include <QApplication>
#include <QWidget>
#include <QScreen>
int
main (int argc, char *argv[])
{
	QApplication app (argc, argv);

	MainWindow mainwindow;

	auto desktop = QGuiApplication::primaryScreen()->availableGeometry();
	mainwindow.move ((desktop.width() - mainwindow.width()) / 2, (desktop.height() - mainwindow.height()) / 2);

	mainwindow.show();

	return app.exec();
}
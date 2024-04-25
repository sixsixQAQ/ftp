#pragma once

#include <QGuiApplication>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QWidget>

namespace Ui {
class MainWindow;
};

class MainWindow : public QWidget {
	Q_OBJECT
public:
	MainWindow (QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void onSocketReadAble ();
private:
	void init ();
	void sendToFrontAndBackend (const QString &cmdText);
	std::shared_ptr<Ui::MainWindow> ui = nullptr;
	QTcpSocket *m_socket;
};
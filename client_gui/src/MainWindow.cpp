#include "MainWindow.hpp"

#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMenu>
#include <QProcess>
#include <QStyle>
#include <QStandardPaths>

MainWindow::MainWindow (QWidget *parent) : QWidget (parent), ui (std::make_shared<Ui::MainWindow>())
{
	init();
	connect (m_socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadAble);
	connect (ui->connectBtn, &QPushButton::clicked, this, [&] {
		const QString openCmd = QString ("open %1 %2\n").arg (ui->ipLineEdit->text()).arg (ui->portLineEdit->text());

		sendToFrontAndBackend (openCmd);
	});

	connect (ui->cmdLineEdit, &QLineEdit::returnPressed, this, [&] {
		if (ui->cmdLineEdit->text() == "quit") {
			this->close();
		}
		ui->cmdTextEdit->insertPlainText (ui->cmdLineEdit->text() + "\n");
		m_socket->write ((ui->cmdLineEdit->text() + "\n").toUtf8());
		
		ui->cmdLineEdit->clear();
	});
	connect (ui->localDirBrowseBtn, &QPushButton::clicked, this, [&] {
		QString dir = QFileDialog::getExistingDirectory (this, tr ("Open Directory"), ui->localDirLineEdit->text());
		if (dir.isEmpty()) {
			return;
		}
		ui->localDirLineEdit->setText (dir);
	});
}

MainWindow::~MainWindow()
{
	m_socket->abort();
}

void
MainWindow::init()
{
	ui->setupUi (this);
	setMinimumSize (1200, 480);

	ui->cmdTextEdit->setContextMenuPolicy (Qt::ContextMenuPolicy::CustomContextMenu);
	connect (ui->cmdTextEdit, &QTextEdit::customContextMenuRequested, this, [&] (const QPoint &pos) {
		Q_UNUSED (pos);
		auto pMenu = new QMenu (this);
		pMenu->addAction ("清空", [&] { ui->cmdTextEdit->setText ("ftp> "); });
		pMenu->exec (cursor().pos());
	});
	ui->passwordLineEdit->setEchoMode (QLineEdit::Password);
	ui->portLineEdit->setText ("21");
	m_socket = new QTcpSocket (this);
	QProcess::startDetached ("client", {"-gui"});
	m_socket->connectToHost ("127.0.0.1", 65535);

	ui->localDirLineEdit->setReadOnly(true);
	ui->localDirLineEdit->setText(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
}

void
MainWindow::sendToFrontAndBackend (const QString &text)
{
	ui->cmdTextEdit->insertPlainText (text);
	m_socket->write (text.toUtf8());
}

void
MainWindow::onSocketReadAble()
{
	QByteArray bytes;
	bytes.resize (m_socket->bytesAvailable());
	m_socket->read (bytes.data(), m_socket->bytesAvailable());

	QString result = bytes;
	if (result.indexOf ("Username:") != -1) {
		result += " " + ui->userLineEdit->text() + "\n";
		m_socket->write ((ui->userLineEdit->text() + "\n").toUtf8());

	} else if (result.indexOf ("Password:") != -1) {
		result += " ********\n";
		m_socket->write ((ui->passwordLineEdit->text() + "\n").toUtf8());
	}


	ui->cmdTextEdit->insertPlainText (result);

	QTextCursor cursor = ui->cmdTextEdit->textCursor();
	cursor.movePosition (QTextCursor::End);
	ui->cmdTextEdit->setTextCursor (cursor);
}

#pragma once
#include <QTextEdit>

class ShellTextEdit : public QTextEdit {
public:
	ShellTextEdit (QWidget *parent = nullptr);

protected:
	void keyPressEvent (QKeyEvent *event) override;
};
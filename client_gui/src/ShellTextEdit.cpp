#include "ShellTextEdit.hpp"

#include <QKeyEvent>

ShellTextEdit::ShellTextEdit (QWidget *parent) : QTextEdit (parent)
{
	setReadOnly (true);
}

void
ShellTextEdit::keyPressEvent (QKeyEvent *event)
{
	// if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
	// 	// QTextCursor cursor = textCursor();
	// 	// cursor.movePosition (QTextCursor::EndOfBlock);
	// 	// setTextCursor (cursor);
	// 	// insertPlainText ("\n");
	// } else {
	// 	QTextEdit::keyPressEvent (event);
	// }

	QTextEdit::keyPressEvent (event);
}

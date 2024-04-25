#include "ClientCmd/quitCmd.hpp"

bool
quitCmd::isControlConnectionNeeded()
{
	return false;
}

std::pair<bool, std::function<void()>>
quitCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1) {
		return {false, [&] () { m_context.outStream << "usage: quit\n"; }};
	}
	return {true, [] () {}};
}

std::string
quitCmd::help()
{
	return "quit\t\t\texit ftp client";
}

void
quitCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	if (m_context.ctrlFd.isOpened())
		sender.QUIT (m_context.ctrlFd);
	m_context.ctrlFd.close();
	m_context.dataFd.close();
	throw quitException();
}

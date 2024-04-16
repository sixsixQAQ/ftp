#include "ClientCmd/deleteCmd.hpp"

std::pair<bool, std::function<void()>>
deleteCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: delete remote-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
deleteCmd::help()
{
	return "delete\t\t\tdelete remote file";
}

void
deleteCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.DELE (m_context.ctrlFd, m_args[1]);
}

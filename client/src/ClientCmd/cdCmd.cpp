#include "ClientCmd/cdCmd.hpp"

std::pair<bool, std::function<void()>>
cdCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: cd remote-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
cdCmd::help()
{
	return "cd\t\t\tchange remote path";
}

void
cdCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.CWD (m_context.ctrlFd, m_args[1]);
}
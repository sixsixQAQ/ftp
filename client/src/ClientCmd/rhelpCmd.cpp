#include "ClientCmd/rhelpCmd.hpp"

std::string
rhelpCmd::help()
{
	return "rhelp\t\t\tshow help";
}

std::pair<bool, std::function<void()>>
rhelpCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1 && m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: rhelp [cmd]\n"; }};
	}
	return {true, [] () {}};
}

void
rhelpCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	if (m_args.size() == 1) {
		sender.HELP (m_context.ctrlFd);

	} else if (m_args.size() == 2) {
		sender.HELP (m_context.ctrlFd, m_args[1]);
	}
}

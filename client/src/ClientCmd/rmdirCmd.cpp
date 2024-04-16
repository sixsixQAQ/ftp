#include "ClientCmd/rmdirCmd.hpp"

std::pair<bool, std::function<void()>>
rmdirCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: rmdir remote-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
rmdirCmd::help()
{
	return "rmdir\t\t\tremove remote die";
}

void
rmdirCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.RMD (m_context.ctrlFd, m_args[1]);
}

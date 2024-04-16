#include "ClientCmd/pwdCmd.hpp"

std::pair<bool, std::function<void()>>
pwdCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1) {
		return {false, [&] () { m_context.outStream << "usage: pwd\n"; }};
	}
	return {true, [] () {}};
}

std::string
pwdCmd::help()
{
	return "pwd\t\t\tshow remote path";
}

void
pwdCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.PWD (m_context.ctrlFd);
}

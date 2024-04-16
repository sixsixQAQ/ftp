#include "ClientCmd/renameCmd.hpp"

std::pair<bool, std::function<void()>>
renameCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 3) {
		return {false, [&] () { m_context.outStream << "usage: rename old-path new-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
renameCmd::help()
{
	return "rename\t\t\trename remote path";
}

void
renameCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.RNFR (m_context.ctrlFd, m_args[1]);
	sender.RNTO (m_context.ctrlFd, m_args[2]);
}

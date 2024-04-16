#include "ClientCmd/mkdirCmd.hpp"

bool
mkdirCmd::isControlConnectionNeeded()
{
	return true;
}

std::pair<bool, std::function<void()>>
mkdirCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: mkdir remote-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
mkdirCmd::help()
{
	return "mkdir\t\t\tcreate remote path";
}

void
mkdirCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.MKD (m_context.ctrlFd, m_args[1]);
}

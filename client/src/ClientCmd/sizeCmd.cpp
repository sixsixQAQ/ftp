#include "ClientCmd/sizeCmd.hpp"

std::pair<bool, std::function<void()>>
sizeCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: size remote-path\n"; }};
	}
	return {true, [] () {}};
}

std::string
sizeCmd::help()
{
	return "size\t\t\tget remote file size";
}

void
sizeCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.SIZE (m_context.ctrlFd, m_args[1]);
}

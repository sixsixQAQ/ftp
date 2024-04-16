#include "ClientCmd/lsCmd.hpp"

#include "BaseUtil.hpp"

bool
lsCmd::isDataConnectionNeeded()
{
	return true;
}

std::string
lsCmd::help()
{
	return "ls\t\t\tlist remote path";
}

std::pair<bool, std::function<void()>>
lsCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1 && m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: ls [remote-path]\n"; }};
	}
	return {true, [] () {}};
}

void
lsCmd::exec()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.setLISTDataCallback ([&] (DataFd dataFd) { m_context.outStream << IOUtil::readAll (dataFd); });
	sender.LIST (m_context.ctrlFd, m_args.size() == 2 ? m_args[1] : "");
}

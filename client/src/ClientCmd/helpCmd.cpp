#include "ClientCmd/helpCmd.hpp"

bool
helpCmd::isControlConnectionNeeded()
{
	return false;
}

std::string
helpCmd::help()
{
	return "help\t\t\tshow help";
}

std::pair<bool, std::function<void()>>
helpCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1 && m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: help [cmd]\n"; }};
	}
	return {true, [] () {}};
}

void
helpCmd::exec()
{

	if (m_args.size() == 1) {
		int i = 1;
		for (auto &cmd : ClientCmd::getFactoryList()) {
			m_context.outStream << cmd.first << "\t\t";
			if ((i++ % 5) == 0)
				m_context.outStream << "\n";
		}
		m_context.outStream << "\n";
	} else {
		auto cmd = cmdFactory (m_context, {m_args[1]});
		if (cmd)
			m_context.outStream << cmd->help() << "\n";
		else
			m_context.outStream << "No such cmd: " << m_args[1] << "\n";
	}
}

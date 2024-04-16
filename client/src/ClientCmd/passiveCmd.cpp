#include "ClientCmd/passiveCmd.hpp"

bool
passiveCmd::isControlConnectionNeeded()
{
	return false;
}

std::pair<bool, std::function<void()>>
passiveCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1) {
		return {false, [&] () { m_context.outStream << "usage: passive\n"; }};
	}
	return {true, [] () {}};
}

std::string
passiveCmd::help()
{
	return "passive\t\t\tenter passive mode";
}

void
passiveCmd::exec()
{
	if (m_context.transferMode == Context::TransferMode::PASV) {
		m_context.transferMode = Context::TransferMode::PORT;
		m_context.outStream << "FTP-Data mode: PORT.\n";
	} else {
		m_context.transferMode = Context::TransferMode::PASV;
		m_context.outStream << "FTP-Data mode: PASV.\n";
	}
}

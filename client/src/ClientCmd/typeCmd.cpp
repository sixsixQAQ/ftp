#include "ClientCmd/typeCmd.hpp"

std::string
typeCmd::help()
{
	return "type\t\t\tset transfer type";
}

std::pair<bool, std::function<void()>>
typeCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 1 && m_args.size() != 2) {
		return {false, [&] () { m_context.outStream << "usage: type [ ascii | binary ]\n"; }};
	}
	return {true, [] () {}};
}

void
typeCmd::exec()
{
	if (m_args.size() == 1) {
		if (m_context.representationType == ProtocolCmdSender::RepresentationType::ASCII)
			m_context.outStream << "Current transfer type: ascii" << std::endl;
		else
			m_context.outStream << "Current transfer type: binary" << std::endl;
		return;
	}

	auto sender = ProtocolCmdSender::insatnce();
	if (m_args[1] == "ascii") {
		// TODO: 全面支持ASCII模式
		m_context.outStream << "Sorry, ascii mode is not supported." << std::endl;
		// m_context.representationType = ProtocolCmdSender::RepresentationType::ASCII;
		// sender.TYPE (m_context.ctrlFd, ProtocolCmdSender::RepresentationType::ASCII);
	} else if (m_args[1] == "binary") {
		m_context.representationType = ProtocolCmdSender::RepresentationType::BINARY;
		sender.TYPE (m_context.ctrlFd, ProtocolCmdSender::RepresentationType::BINARY);
	}
}

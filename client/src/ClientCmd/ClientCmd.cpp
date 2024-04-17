#include "ClientCmd/ClientCmd.hpp"

#include "BaseUtil.hpp"
#include "FTPResponseUtil.hpp"
#include "ProtocolCmdSender.hpp"

ClientCmd::ClientCmd (Context &context, const ArgList &args) : m_context (context), m_args (args)
{
}

std::shared_ptr<ClientCmd>
ClientCmd::cmdFactory (Context &context, const ArgList &args)
{
	if (args.size() == 0) {
		return nullptr;
	}
	auto factoryList = getFactoryList();
	for (auto it : factoryList) {
		if (it.first == args[0]) {
			return it.second (context, args);
		}
	}
	return nullptr;
}

void
ClientCmd::templateExecution()
{
	if (isControlConnectionNeeded()) {
		if (!m_context.ctrlFd.isOpened()) {
			m_context.outStream << "Not connected.\n";
			return;
		}
	}

	auto checkResult = checkArgsWhenAboutToExec();
	if (checkResult.first == false) {
		checkResult.second();
		return;
	}

	if (isDataConnectionNeeded()) {
		if (m_context.transferMode == Context::TransferMode::PASV) {
			enterPASVMode();
		} else {
			enterPORTMode();
		}
	}

	exec();
}

bool
ClientCmd::isDataConnectionNeeded()
{
	return false;
}

bool
ClientCmd::isControlConnectionNeeded()
{
	return true;
}

void
ClientCmd::enterPASVMode()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.setPostWaitForResponse ([&] (std::vector<std::string> response) {
		if (response.empty())
			return;
		FTPResponseUtil::PASVResponse (response[0], m_context.passiveIp, m_context.passivePort);
	});
	sender.PASV (m_context.ctrlFd);
	sender.setPostWaitForResponse ([&] (std::vector<std::string> response) {
		for (auto str : response) {
			m_context.outStream << str;
		}
	});
	sender.setCreateDataConnection ([&] {
		return NetUtil::connectToServer (m_context.passiveIp, m_context.passivePort);
	});
}

void
ClientCmd::enterPORTMode()
{
	// TODO: 支持PORT模式
}

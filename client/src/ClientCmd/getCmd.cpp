#include "ClientCmd/getCmd.hpp"

#include "BaseUtil.hpp"

bool
getCmd::isDataConnectionNeeded()
{
	return true;
}

std::string
getCmd::help()
{
	return "get\t\t\tdownload remote file";
}

std::pair<bool, std::function<void()>>
getCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 3) {
		return {false, [&] () { m_context.outStream << "usage: get remote-path local-path\n"; }};
	}
	return {true, [] () {}};
}

void
getCmd::exec()
{
	const std::string &localPath  = m_args[2];
	const std::string &remotePath = m_args[1];
	auto sender					  = ProtocolCmdSender::insatnce();
	sender.setRETRDataCallback ([&] (DataFd dataFd) {
		NetUtil::syncRemoteToLocal (dataFd, localPath, [&] (size_t syncedSize) {
			auto str = "\rWhole Size:" + std::to_string (0 / 1024.0 / 1024.0) +
					   "MB, transfered:" + std::to_string (syncedSize / 1024.0 / 1024.0) + "MB";
			m_context.outStream << str << std::flush;
		});
		m_context.outStream << "\n";
	});
	sender.RETR (m_context.ctrlFd, remotePath);
}

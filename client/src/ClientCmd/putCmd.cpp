#include "ClientCmd/putCmd.hpp"

#include "BaseUtil.hpp"

bool
putCmd::isDataConnectionNeeded()
{
	return true;
}

std::string
putCmd::help()
{
	return "put\t\t\tupload local file";
}

std::pair<bool, std::function<void()>>
putCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 3) {
		return {false, [&] () { m_context.outStream << "usage: put local-path remote-path\n"; }};
	}

	const long wholeSize = IOUtil::getFileSize (m_args[1]);
	if (wholeSize == -1) {
		return {false, [&] () { m_context.outStream << "No such file: " << m_args[1] << "\n"; }};
	}

	return {true, [] () {}};
}

void
putCmd::exec()
{
	const std::string &localPath  = m_args[1];
	const std::string &remotePath = m_args[2];

	auto sender = ProtocolCmdSender::insatnce();

	const long wholeSize = IOUtil::getFileSize (localPath);
	sender.setSTORDataCallback ([&] (DataFd dataFd) {
		NetUtil::syncLocalToRemote (dataFd, localPath, [&] (size_t syncedSize) {
			auto str = "\rWhole Size:" + std::to_string (wholeSize / 1024.0 / 1024.0) +
					   "MB, transfered:" + std::to_string (syncedSize / 1024.0 / 1024.0) + "MB";
			m_context.outStream << str << std::flush;
		});
		m_context.outStream << "\n";
	});
	sender.STOR (m_context.ctrlFd, remotePath);
}

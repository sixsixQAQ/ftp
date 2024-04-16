#include "ClientCmd/openCmd.hpp"

#include "BaseUtil.hpp"
#include "FTPResponseUtil.hpp"

bool
openCmd::isControlConnectionNeeded()
{
	return false;
}

std::pair<bool, std::function<void()>>
openCmd::checkArgsWhenAboutToExec()
{
	if (m_args.size() != 2 && m_args.size() != 3) {
		return {false, [&] () { m_context.outStream << "usage: open host-name [port]\n"; }};
	}
	return {true, [] () {}};
}

std::string
openCmd::help()
{
	return "open\t\t\tconnect to remote ftp server";
}

void
openCmd::exec()
{
	initFixedCallback();

	if (m_args.size() == 2)
		m_context.ctrlFd = NetUtil::connectToServer (m_args[1], 21);
	else if (m_args.size() == 2)
		m_context.ctrlFd = NetUtil::connectToServer (m_args[1], std::atoi (m_args[2].c_str()));
	if (m_context.ctrlFd == -1) {
		m_context.outStream << "Failed to connect.\n";
		return;
	}
	FTPResponseUtil::echoResponse (m_context.ctrlFd, m_context.outStream);

	auto sender = ProtocolCmdSender::insatnce();

	{
		m_context.outStream << "Username:";
		std::string userName;
		getline (m_context.inStream, userName);
		sender.USER (m_context.ctrlFd, userName);
	}
	{
		m_context.outStream << "Password:";
		std::string password;
		getline (m_context.inStream, password);
		sender.PASS (m_context.ctrlFd, password);
	}
	// TODO:这里根据响应码记录用户是否登录成功、记录登录信息
}

void
openCmd::initFixedCallback()
{
	auto sender = ProtocolCmdSender::insatnce();
	sender.setPreWaitForResponse ([&] {
		// do nothing
	});
	sender.setPostWaitForResponse ([&] (std::vector<std::string> response) {
		for (auto str : response) {
			m_context.outStream << str;
		}
	});

	sender.setLISTDataCallback ([&] (DataFd fd) { m_context.outStream << IOUtil::readAll (fd); });
	sender.setNLSTDataCallback ([&] (DataFd fd) { m_context.outStream << IOUtil::readAll (fd); });
}

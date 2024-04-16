#pragma once

#include "AbstractFd.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class ProtocolCmdSender {
public:
	static ProtocolCmdSender &insatnce ()
	{
		static ProtocolCmdSender instance;
		return instance;
	}

	ProtocolCmdSender();
	ProtocolCmdSender &setPreWaitForResponse (std::function<void()> preWaitForResponse);
	ProtocolCmdSender &setPostWaitForResponse (std::function<void (std::vector<std::string> response)> postWait);
	ProtocolCmdSender &setCreateDataConnection (std::function<DataFd()> createDataConnection);

	//针对数据的回调
	ProtocolCmdSender &setLISTDataCallback (std::function<void (DataFd)> listDataCallback);
	ProtocolCmdSender &setNLSTDataCallback (std::function<void (DataFd)> listDataCallback);
	ProtocolCmdSender &setRETRDataCallback (std::function<void (DataFd)> retrDataCallback);
	ProtocolCmdSender &setSTORDataCallback (std::function<void (DataFd)> storDataCallback);
public:

	void USER (ControlFd connFd, const std::string &userName);
	void PASS (ControlFd connFd, const std::string &passwd);
	void PWD (ControlFd connFd);
	void LIST (ControlFd connFd, const std::string &remotePath = "");
	void NLST (ControlFd connFd, const std::string &remotePath = "");
	void CWD (ControlFd connFd, const std::string &remotePath);
	void MKD (ControlFd connFd, const std::string &remotePath);
	void RMD (ControlFd connFd, const std::string &remotePath);
	void DELE (ControlFd connFd, const std::string &remotePath);
	void SYST (ControlFd connFd);
	void RETR (ControlFd connFd, const std::string &remotePath);
	void STOR (ControlFd connFd, const std::string &remotePath);
	void SIZE (ControlFd connFd, const std::string &remotePath);
	void RNFR (ControlFd connFd, const std::string &remotePath);
	void RNTO (ControlFd connFd, const std::string &remotePath);
	void QUIT (ControlFd connFd);
	void PASV (ControlFd connFd);
	void NOOP (ControlFd connFd);
	void HELP (ControlFd connFd, const std::string &command = "");

	enum class RepresentationType { ASCII, BINARY };
	void TYPE (ControlFd connFd, RepresentationType type);
	// void PORT (int connnFd, uint32_t ip, uint16_t port);
private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

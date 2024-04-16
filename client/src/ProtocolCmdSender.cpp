#include "ProtocolCmdSender.hpp"

#include "BaseUtil.hpp"
#include "FTPResponseUtil.hpp"
#include "FTPUtil.hpp"

class ProtocolCmdSender::Impl {
public:
	Impl (ProtocolCmdSender &parent) : m_parent (parent)
	{
	}

	ProtocolCmdSender &setPreWaitForResponse (std::function<void()> preWaitForResponse)
	{
		m_preWait = preWaitForResponse;
		return m_parent;
	}

	ProtocolCmdSender &setPostWaitForResponse (
		std::function<void (std::vector<std::string> response)> postWaitForResponse
	)
	{
		m_postWait = postWaitForResponse;
		return m_parent;
	}

	ProtocolCmdSender &setCreateDataConnection (std::function<DataFd()> createDataConnection)
	{
		m_createDataConnection = createDataConnection;
		return m_parent;
	}

	ProtocolCmdSender &setLISTDataCallback (std::function<void (DataFd)> listDataCallback)
	{
		m_LISTCallback = listDataCallback;
		return m_parent;
	}

	ProtocolCmdSender &setNLSTDataCallback (std::function<void (DataFd)> listDataCallback)
	{
		m_NLSTCallback = listDataCallback;
		return m_parent;
	}

	ProtocolCmdSender &setRETRDataCallback (std::function<void (DataFd)> retrDataCallback)
	{
		m_RETRCallback = retrDataCallback;
		return m_parent;
	}

	ProtocolCmdSender &setSTORDataCallback (std::function<void (DataFd)> storDataCallback)
	{
		m_STORCallback = storDataCallback;
		return m_parent;
	}

	void USER (ControlFd connFd, const std::string &userName);
	void PASS (ControlFd connFd, const std::string &passwd);
	void PWD (ControlFd connFd);
	void LIST (ControlFd connFd, const std::string &remotePath);
	void NLST (ControlFd connFd, const std::string &remotePath);
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
	void TYPE (ControlFd connFd, RepresentationType type);
	void NOOP (ControlFd connFd);
	void HELP (ControlFd connFd, const std::string &command);
private:

	void preWait ()
	{
		if (m_preWait)
			m_preWait();
	}

	void postWait (std::vector<std::string> response)
	{
		if (m_postWait)
			m_postWait (response);
	}

	DataFd createDataConnection ()
	{
		if (m_createDataConnection)
			return m_createDataConnection();
		return -1;
	}

	void LISTCallback (DataFd dataFd)
	{
		if (m_LISTCallback)
			m_LISTCallback (dataFd);
	}

	void NLSTCallback (DataFd dataFd)
	{
		if (m_NLSTCallback)
			m_NLSTCallback (dataFd);
	}

	void RETRCallback (DataFd dataFd)
	{
		if (m_RETRCallback)
			m_RETRCallback (dataFd);
	}

	void STORCallback (DataFd dataFd)
	{
		if (m_STORCallback)
			m_STORCallback (dataFd);
	}

	std::function<void()> m_preWait									   = nullptr;
	std::function<void (std::vector<std::string> response)> m_postWait = nullptr;
	std::function<DataFd()> m_createDataConnection					   = nullptr;
	std::function<void (DataFd)> m_LISTCallback						   = nullptr;
	std::function<void (DataFd)> m_NLSTCallback						   = nullptr;
	std::function<void (DataFd)> m_RETRCallback						   = nullptr;
	std::function<void (DataFd)> m_STORCallback						   = nullptr;
	ProtocolCmdSender &m_parent;
};

void
ProtocolCmdSender::Impl::USER (ControlFd connFd, const std::string &userName)
{
	FTPUtil::sendCmd (connFd, {"USER", userName});

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::PASS (ControlFd connFd, const std::string &passwd)
{
	FTPUtil::sendCmd (connFd, {"PASS", passwd});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::PWD (ControlFd connFd)
{
	FTPUtil::sendCmd (connFd, {"PWD"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::LIST (ControlFd connFd, const std::string &remotePath)
{
	DataFd dataFd = createDataConnection();

	if (remotePath.empty()) {
		FTPUtil::sendCmd (connFd, {"LIST"});
	} else {
		FTPUtil::sendCmd (connFd, {"LIST", remotePath});
	}

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	LISTCallback (dataFd);


	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	dataFd.close();
}

void
ProtocolCmdSender::Impl::NLST (ControlFd connFd, const std::string &remotePath)
{
	DataFd dataFd = createDataConnection();

	if (remotePath.empty()) {
		FTPUtil::sendCmd (connFd, {"NLST"});
	} else {
		FTPUtil::sendCmd (connFd, {"NLST", remotePath});
	}

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	NLSTCallback (dataFd);

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	dataFd.close();
}

void
ProtocolCmdSender::Impl::CWD (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"CWD", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::MKD (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"MKD", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::RMD (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"RMD", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::DELE (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"DELE", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::SYST (ControlFd connFd)
{
	FTPUtil::sendCmd (connFd, {"SYST"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::RETR (ControlFd connFd, const std::string &remotePath)
{
	DataFd dataFd = createDataConnection();

	FTPUtil::sendCmd (connFd, {"RETR", remotePath});

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	RETRCallback (dataFd);
	dataFd.close();

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::STOR (ControlFd connFd, const std::string &remotePath)
{
	DataFd dataFd = createDataConnection();
	FTPUtil::sendCmd (connFd, {"STOR", remotePath});

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));

	STORCallback (dataFd);
	dataFd.close();

	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::SIZE (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"SIZE", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::RNFR (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"RNFR", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::RNTO (ControlFd connFd, const std::string &remotePath)
{
	FTPUtil::sendCmd (connFd, {"RNTO", remotePath});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::QUIT (ControlFd connFd)
{
	FTPUtil::sendCmd (connFd, {"QUIT"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::PASV (ControlFd connFd)
{
	FTPUtil::sendCmd (connFd, {"PASV"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::TYPE (ControlFd connFd, RepresentationType type)
{
	FTPUtil::sendCmd (connFd, {"TYPE", type == RepresentationType::ASCII ? "A" : "I"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::NOOP (ControlFd connFd)
{
	FTPUtil::sendCmd (connFd, {"NOOP"});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

void
ProtocolCmdSender::Impl::HELP (ControlFd connFd, const std::string &command)
{
	FTPUtil::sendCmd (connFd, {"HELP", command});
	preWait();
	postWait (FTPResponseUtil::getResponse (connFd));
}

///////////////////////////////////////////////////////////////////////////////////
ProtocolCmdSender::ProtocolCmdSender() : m_pImpl (std::make_shared<Impl> (*this))
{
}

ProtocolCmdSender &
ProtocolCmdSender::setPreWaitForResponse (std::function<void()> preWait)
{
	if (m_pImpl)
		m_pImpl->setPreWaitForResponse (preWait);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setPostWaitForResponse (std::function<void (std::vector<std::string> response)> postWait)
{
	if (m_pImpl)
		m_pImpl->setPostWaitForResponse (postWait);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setCreateDataConnection (std::function<DataFd()> createDataConnection)
{
	if (m_pImpl)
		m_pImpl->setCreateDataConnection (createDataConnection);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setLISTDataCallback (std::function<void (DataFd)> listDataCallback)
{
	if (m_pImpl)
		m_pImpl->setLISTDataCallback (listDataCallback);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setNLSTDataCallback (std::function<void (DataFd)> listDataCallback)
{
	if (m_pImpl)
		m_pImpl->setNLSTDataCallback (listDataCallback);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setRETRDataCallback (std::function<void (DataFd)> retrDataCallback)
{
	if (m_pImpl)
		m_pImpl->setRETRDataCallback (retrDataCallback);
	return *this;
}

ProtocolCmdSender &
ProtocolCmdSender::setSTORDataCallback (std::function<void (DataFd)> storDataCallback)
{
	if (m_pImpl)
		m_pImpl->setSTORDataCallback (storDataCallback);
	return *this;
}

void
ProtocolCmdSender::USER (ControlFd connFd, const std::string &userName)
{
	if (m_pImpl)
		m_pImpl->USER (connFd, userName);
}

void
ProtocolCmdSender::PASS (ControlFd connFd, const std::string &passwd)
{
	if (m_pImpl)
		m_pImpl->PASS (connFd, passwd);
}

void
ProtocolCmdSender::CWD (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->CWD (connFd, remotePath);
}

void
ProtocolCmdSender::RNFR (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->RNFR (connFd, remotePath);
}

void
ProtocolCmdSender::RNTO (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->RNTO (connFd, remotePath);
}

void
ProtocolCmdSender::QUIT (ControlFd connFd)
{
	if (m_pImpl)
		m_pImpl->QUIT (connFd);
}

void
ProtocolCmdSender::PASV (ControlFd connFd)
{
	if (m_pImpl)
		m_pImpl->PASV (connFd);
}

void
ProtocolCmdSender::NOOP (ControlFd connFd)
{
	if (m_pImpl)
		m_pImpl->NOOP (connFd);
}

void
ProtocolCmdSender::HELP (ControlFd connFd, const std::string &command)
{
	if (m_pImpl)
		m_pImpl->HELP (connFd, command);
}

void
ProtocolCmdSender::TYPE (ControlFd connFd, RepresentationType type)
{
	if (m_pImpl)
		m_pImpl->TYPE (connFd, type);
}

void
ProtocolCmdSender::DELE (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->DELE (connFd, remotePath);
}

void
ProtocolCmdSender::RMD (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->RMD (connFd, remotePath);
}

void
ProtocolCmdSender::MKD (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->MKD (connFd, remotePath);
}

void
ProtocolCmdSender::LIST (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->LIST (connFd, remotePath);
}

void
ProtocolCmdSender::NLST (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->NLST (connFd, remotePath);
}

void
ProtocolCmdSender::PWD (ControlFd connFd)
{
	if (m_pImpl)
		m_pImpl->PWD (connFd);
}

void
ProtocolCmdSender::SIZE (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->SIZE (connFd, remotePath);
}

void
ProtocolCmdSender::SYST (ControlFd connFd)
{
	if (m_pImpl)
		m_pImpl->SYST (connFd);
}

void
ProtocolCmdSender::RETR (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->RETR (connFd, remotePath);
}

void
ProtocolCmdSender::STOR (ControlFd connFd, const std::string &remotePath)
{
	if (m_pImpl)
		m_pImpl->STOR (connFd, remotePath);
}

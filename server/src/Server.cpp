#include "Server.hpp"

#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "Toggle.hpp"

#include <assert.h>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <unistd.h>

class Server::Impl {
public:
	struct Context {
		ControlFd ctrlFd = -1;
		DataFd dataFd	 = -1;
		Toggle PASVToggle;
		struct sockaddr_in clientAddr;
	};

	Impl (EasySelect *dispatcher, int ctrlFd, struct sockaddr_in addr);
	void notifyDataCome ();


private:
	EasySelect *m_dispatcher;
	Context m_context;

	void work ();
	void stopService ();

	std::mutex m_mutex;
	std::condition_variable m_dataCome;
	bool m_haveData = false;
};

Server::Impl::Impl (EasySelect *dispatcher, int ctrlFd, sockaddr_in addr) : m_dispatcher (dispatcher)
{
	m_context.clientAddr = addr;
	m_context.PASVToggle.turnOff();
	m_context.ctrlFd = ctrlFd;

	auto workThread = std::thread (&Impl::work, this);
	workThread.detach();
}

void
Server::Impl::notifyDataCome()
{
	std::lock_guard<std::mutex> lock (m_mutex);
	m_haveData = true;
	m_dataCome.notify_one();
}

void
Server::Impl::work()
{
	BackableReader ctrlReader (m_context.ctrlFd);

	while (true) {
		{
			std::unique_lock<std::mutex> lock (m_mutex);
			m_dataCome.wait (lock, [&] { return m_haveData; });
			m_haveData = false;
		}

		char buf[4096];
		int64_t nRead = ctrlReader.read (buf, sizeof (buf));
		if (nRead == 0) {
			stopService();
			return;
		} else if (nRead < 0) {
			stopService();
			setError (strerror (errno));
			return;
		} else {
			int fd = m_context.ctrlFd;

			std::string userCmd = "USER";
			std::string passCmd = "PASS";

			std::stringstream stream;
			stream.write (buf, nRead);
			std::string request = stream.str();
			std::cerr << request << "\n";
			if (request.compare (0, userCmd.length(), userCmd) == 0) {
				std::string reply = "331 Password,please.\r\n";
				IOUtil::writen (fd, reply.c_str(), reply.length());
				std::cerr << reply;
			} else if (request.compare (0, passCmd.length(), passCmd) == 0) {
				std::string reply = "230 Login Ok.\r\n";
				IOUtil::writen (fd, reply.c_str(), reply.length());
				std::cerr << reply;
			} else {
				std::string reply = "502 cmd is not implemented\r\n";
				IOUtil::writen (fd, reply.c_str(), reply.length());
				std::cerr << reply;
			}

			IOUtil::writen (STDOUT_FILENO, buf, nRead);
		}
	}
}

void
Server::Impl::stopService()
{
	m_context.ctrlFd.close();
	m_context.dataFd.close();

	m_dispatcher->stop();
	m_dispatcher->removeFd (m_context.ctrlFd);
	m_dispatcher->start();
}

////////////////////////////////////////////////////////////////////////////
Server::Server (EasySelect *dispatcher, int ctrlFd, sockaddr_in addr) :
	m_pImpl (std::make_shared<Impl> (dispatcher, ctrlFd, addr))
{
	assert (m_pImpl);
}

void
Server::notifyDataCome()
{
	assert (m_pImpl);
	return m_pImpl->notifyDataCome();
}

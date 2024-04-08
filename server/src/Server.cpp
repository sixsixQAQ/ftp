#include "Server.hpp"

#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "ClientContext.hpp"
#include "RequestHandler.hpp"
#include "ServerUtil.hpp"
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
	Impl (EasySelect *dispatcher, int ctrlFd, struct sockaddr_in addr);
	void notifyDataCome ();


private:
	EasySelect *m_dispatcher;
	ClientContext m_context;

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
	RequestHandler handler (m_context);
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

			std::vector<std::string> request = RequestUtil::parseOneFullRequest_v2 (ctrlReader, buf, nRead);
			if (request.empty())
				continue;
			handler.exec (request);
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

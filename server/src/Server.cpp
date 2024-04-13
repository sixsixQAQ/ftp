#include "Server.hpp"

#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "ClientContext.hpp"
#include "RequestHandler.hpp"
#include "ServerUtil.hpp"
#include "Toggle.hpp"

#include <algorithm>
#include <assert.h>
#include <cassert>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <unistd.h>

class Server::Impl {
public:
	Impl (Server *server)
	{
		assert (server);
		m_server		= server;
		auto workThread = std::thread (&Impl::threadEntry, this);
		workThread.detach();
		dispatcher.waitForStartCompleted();
	}

	void addClient (int fd, struct sockaddr_in addr)
	{
		m_server->preAdd (fd, addr);
		dispatcher.addFd (fd);

		dispatcher.stop();
	}

	void removeClient (int fd)
	{
		dispatcher.removeFd (fd);

		dispatcher.stop();
		m_server->postRemove (fd);
	}

private:

	void threadEntry ()
	{
		m_threadId = std::this_thread::get_id();
		while (true) {
			std::vector<int> readAbleFds = dispatcher.waitForReadAble();

			std::for_each (readAbleFds.begin(), readAbleFds.end(), [&] (int fd) { m_server->workWhenDataCome (fd); });
		}
	}

	std::thread::id m_threadId;
	EasySelect dispatcher;
	Server *m_server;
};

Server::Server() : m_pImpl (std::make_shared<Impl> (this))
{
	assert (m_pImpl);
}

void
Server::addClient (int fd, sockaddr_in addr)
{
	assert (m_pImpl);
	m_pImpl->addClient (fd, addr);
}

void
Server::removeClient (int fd)
{
	assert (m_pImpl);
	m_pImpl->removeClient (fd);
}

void
Server::preAdd (int fd, sockaddr_in addr)
{
}

void
Server::postRemove (int fd)
{
}

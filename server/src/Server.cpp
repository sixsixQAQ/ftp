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

class ControlServer::Impl {
public:
	Impl (Server *server) : m_server (server)
	{
		assert (m_server);
	}
public:
	void preAdd (int fd, struct sockaddr_in addr)
	{
		std::string welcome = "220 FreeFtp-server v1.0\r\n";
		IOUtil::writen (fd, welcome.c_str(), welcome.size());

		std::lock_guard<std::mutex> lock (m_contextMutex);
		ClientContext context;
		context.PASVToggle.turnOff();
		context.ctrlFd	   = fd;
		context.isLogined  = false;
		context.clientAddr = addr;
		context.ctrlReader = BackableReader (fd);
		m_contextMap.insert ({fd, context});
	}

	void postRemove (int fd)
	{
		std::lock_guard<std::mutex> lock (m_contextMutex);
		m_contextMap.erase (fd);
	}

	void stopService (int fd)
	{
		{
			std::lock_guard<std::mutex> lock (m_contextMutex);
			auto contextIt = m_contextMap.find (fd);
			if (contextIt == m_contextMap.end())
				return;

			ClientContext &context = contextIt->second;
			context.ctrlFd.close();
			context.dataFd.close();
		}
		m_server->removeClient (fd);
	}

	void workWhenDataCome (int fd)
	{
		std::unique_lock<std::mutex> lock (m_contextMutex);
		auto clientIt = m_contextMap.find (fd);
		if (clientIt == m_contextMap.end()) {
			stopService (fd);
			return;
		}

		ClientContext &context = clientIt->second;
		RequestHandler handler (context);
		char buf[4096];
		int64_t nRead = context.ctrlReader.read (buf, sizeof (buf));
		if (nRead == 0) {
			lock.unlock();
			stopService (fd);
			return;
		} else if (nRead < 0) {
			lock.unlock();
			stopService (fd);
			setError (strerror (errno));
			return;
		} else {

			std::vector<std::string> request = RequestUtil::parseOneFullRequest_v2 (context.ctrlReader, buf, nRead);
			if (!request.empty())
				handler.exec (request);
		}
	}

	Server *m_server;
	std::mutex m_contextMutex;
	std::unordered_map<int, ClientContext> m_contextMap;
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

ControlServer::ControlServer() : m_pImpl (std::make_shared<Impl> (this))
{
}

void
ControlServer::preAdd (int fd, sockaddr_in addr)
{
	assert (m_pImpl);
	return m_pImpl->preAdd (fd, addr);
}

void
ControlServer::postRemove (int fd)
{

	assert (m_pImpl);
	return m_pImpl->postRemove (fd);
}

void
ControlServer::workWhenDataCome (int fd)
{
	assert (m_pImpl);
	return m_pImpl->workWhenDataCome (fd);
}
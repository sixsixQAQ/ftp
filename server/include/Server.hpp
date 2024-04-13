#pragma once

#include "BaseUtil.hpp"
#include "ClientContext.hpp"
#include "EasySelect.hpp"
#include "RequestHandler.hpp"
#include "ServerUtil.hpp"
#include "cstring"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <thread>
#include <unordered_map>

class Server : public ErrorUtil {
public:
	Server();
	void addClient (int fd, struct sockaddr_in addr);
	void removeClient (int fd);
protected:
	virtual void preAdd (int fd, struct sockaddr_in addr);
	virtual void postRemove (int fd);
	virtual void workWhenDataCome (int fd) = 0;
private:

	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

class ControlServer : public Server {
public:
	ControlServer()
	{
	}
private:
	void preAdd (int fd, struct sockaddr_in addr) override
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

	void postRemove (int fd) override
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
		removeClient (fd);
	}

	void workWhenDataCome (int fd) override
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

	std::mutex m_contextMutex;
	std::unordered_map<int, ClientContext> m_contextMap;
};

class DataServer : public Server {
public:

private:
	void workWhenDataCome (int fd) override
	{
	}
};
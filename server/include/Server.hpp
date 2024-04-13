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
	ControlServer();
protected:
	void preAdd (int fd, struct sockaddr_in addr) override;
	void postRemove (int fd) override;
	void workWhenDataCome (int fd) override;
private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

class DataServer : public Server {
public:
	DataServer()
	{
	}

	static DataServer &instance ()
	{
		static DataServer server;
		return server;
	}

	void addUploadTask (int fd, const std::string &localPath)
	{
		std::lock_guard<std::mutex> lock (m_mapMutex);
		m_uploadMap.insert ({fd, localPath});
	}

	void sendOnly (int fd, const std::string &text)
	{
		IOUtil::writen (fd, text.c_str(), text.length());
	}

	void addDownloadTask (int fd, const std::string &localPath)
	{
		std::thread th ([&] { NetUtil::syncLocalToRemote (fd, localPath); });
		th.join();
	}

private:

	void workWhenDataCome (int fd) override
	{
		std::string localPath;
		{
			std::lock_guard<std::mutex> lock (m_mapMutex);
			auto pairIt = m_uploadMap.find (fd);
			if (pairIt == m_uploadMap.end()) {
				return;
			}
			localPath = pairIt->second;
			m_uploadMap.erase (pairIt);
		}
		NetUtil::syncRemoteToLocal (fd, localPath);
	}

	std::mutex m_mapMutex;
	std::unordered_map<int, std::string> m_uploadMap;
};
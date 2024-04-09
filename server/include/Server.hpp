#pragma once

#include "EasySelect.hpp"

#include <memory>
#include <netinet/in.h>

class ControlServer : public ErrorUtil {
public:
	ControlServer (EasySelect *dispatcher, int ctrlFd, struct sockaddr_in addr);

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

class Server : ErrorUtil {
public:
	Server()
	{
		auto workThread = std::thread (&Server::threadEntry, this);
		workThread.detach();
	}

	void addClient (int fd, struct sockaddr_in addr)
	{
		dispatcher.stop();
		dispatcher.addFd (fd, [&] (int fd) { m_dataCome.notify_one(); });
		dispatcher.start();
	}

	void removeClient (int fd)
	{
		dispatcher.stop();
		dispatcher.removeFd (fd);
		dispatcher.start();
	}

protected:
	virtual void doWork () = 0;
private:

	void waitForData ()
	{
		std::unique_lock<std::mutex> lock (m_haveDataMutex);
		m_dataCome.wait (lock, [&] { return m_haveData; });
		m_haveData = false;
	}

	void threadEntry ()
	{
		while (true) {
			waitForData();
			doWork();
		}
	}

	std::mutex m_haveDataMutex;
	EasySelect dispatcher;
	std::condition_variable m_dataCome;
	bool m_haveData;
};
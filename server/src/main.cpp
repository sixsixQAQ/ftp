#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "EasySelect.hpp"
#include "Toggle.hpp"

#include <arpa/inet.h>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

struct ServiceContext : public ErrorUtil {

	ServiceContext (EasySelect *dispatcher, int ctrlFd, struct sockaddr_in addr) :
		m_dispatcher (dispatcher), m_ctrlFd (ctrlFd)
	{
		m_clientAddr = addr;
		m_PASVToggle.turnOff();

		m_workThread = std::thread (&ServiceContext::work, this);
		m_workThread.detach();
	}

	void notifyTaskCome ()
	{
		std::lock_guard<std::mutex> lock (m_mutex);
		m_haveData = true;
		m_dataCome.notify_one();
	}

	void work ()
	{
		BackableReader ctrlReader (m_ctrlFd);

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
				int fd = m_ctrlFd;

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

	void stopService ()
	{
		m_ctrlFd.close();
		m_dataFd.close();

		m_dispatcher->stop();
		m_dispatcher->removeFd (m_ctrlFd);
		m_dispatcher->start();
	}

	bool isEmpty ()
	{
		return m_dispatcher == nullptr;
	}

	EasySelect *m_dispatcher;
	ControlFd m_ctrlFd = -1;
	DataFd m_dataFd	   = -1;
	Toggle m_PASVToggle;
	struct sockaddr_in m_clientAddr;
private:
	std::mutex m_mutex;
	std::thread m_workThread;
	std::condition_variable m_dataCome;
	bool m_haveData = false;
};

// void
// solveRequest (ServiceContext *context)
// {

// }

void
asyncServe (ServiceContext *context)
{
	if (context == nullptr || context->isEmpty()) {
		return;
	}

	context->notifyTaskCome();
}

void
addToDispatcher (int connFd, struct sockaddr_in clientAddr)
{
	static EasySelect ctrlFdDispatcher;

	auto data = new ServiceContext (&ctrlFdDispatcher, connFd, clientAddr);

	ctrlFdDispatcher.stop();
	ctrlFdDispatcher.addFd (connFd, [&] (int fd) { asyncServe (data); });
	ctrlFdDispatcher.start();

	std::string welcome = "220 FreeFtp-server v1.0\r\n";
	IOUtil::writen (connFd, welcome.c_str(), welcome.size());
}

void
start ()
{
	struct sockaddr_in servAddr;
	servAddr.sin_family		 = AF_INET;
	servAddr.sin_port		 = htons (21);
	servAddr.sin_addr.s_addr = INADDR_ANY;

	int listenFd = socket (AF_INET, SOCK_STREAM, 0);
	if (listenFd == -1) {
		perror ("socket()");
		return;
	}

	if (bind (listenFd, (struct sockaddr *)&servAddr, sizeof (servAddr)) == -1) {
		close (listenFd);
		perror ("bind()");
		return;
	}
	constexpr int LISTEN_QUEUE_SIZE = 10;
	if (listen (listenFd, LISTEN_QUEUE_SIZE) == -1) {
		close (listenFd);
		perror ("listen()");
		return;
	}

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	while (1) {
		int connfd = accept (listenFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (connfd == -1) {
			perror ("accept()");
			continue;
		} else {
			addToDispatcher (connfd, clientAddr);
		}
	}
}

int
main (void)
{
	std::cout << "FreeFTP-Server v1.0\n";
	start();
}

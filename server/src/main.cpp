#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "EasySelect.hpp"
#include "Server.hpp"
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

void
addToControlConnDispatcher (int connFd, struct sockaddr_in clientAddr)
{
	static EasySelect ctrlFdDispatcher;

	auto server = new ControlServer (&ctrlFdDispatcher, connFd, clientAddr);

	ctrlFdDispatcher.stop();
	ctrlFdDispatcher.addFd (connFd, [&] (int fd) { server->notifyDataCome(); });
	ctrlFdDispatcher.start();

	std::string welcome = "220 FreeFtp-server v1.0\r\n";
	IOUtil::writen (connFd, welcome.c_str(), welcome.size());
}

void
addToDataConnDispatcher (int connFd, struct sockaddr_in clientAddr)
{
	static EasySelect dataFdDispatcher;

	auto server = new ControlServer (&dataFdDispatcher, connFd, clientAddr);

	dataFdDispatcher.stop();
	dataFdDispatcher.addFd (connFd, [&] (int fd) { server->notifyDataCome(); });
	dataFdDispatcher.start();

	std::string welcome = "220 FreeFtp-server v1.0\r\n";
	IOUtil::writen (connFd, welcome.c_str(), welcome.size());
}

void
solveConrtrolConnection ()
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
			addToControlConnDispatcher (connfd, clientAddr);
		}
	}
}

void
solveDataConnection ()
{
	struct sockaddr_in servAddr;
	servAddr.sin_family		 = AF_INET;
	servAddr.sin_port		 = htons (65533);
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
			addToControlConnDispatcher (connfd, clientAddr);
		}
	}
}

int
main (void)
{
	std::cout << "FreeFTP-Server v1.0\n";
	solveConrtrolConnection();
}

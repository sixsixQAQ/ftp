#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "BaseUtil.hpp"
#include "EasySelect.hpp"
#include "Server.hpp"

#include <arpa/inet.h>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <signal.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void
addToControlServer (int connFd, struct sockaddr_in clientAddr)
{
	static Server *server = new ControlServer();
	server->addClient (connFd, clientAddr);
}

void
whileConnectionCome (uint16_t port, std::function<void (int connFd, struct sockaddr_in clientAddr)> callback)
{
	struct sockaddr_in servAddr;
	servAddr.sin_family		 = AF_INET;
	servAddr.sin_port		 = htons (port);
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
			callback (connfd, clientAddr);
		}
	}
}

void
solveConrtrolConnection ()
{
	whileConnectionCome (21, addToControlServer);
}

void
solveDataConnection ()
{
	whileConnectionCome (65533, [] (int connFd, struct sockaddr_in clientAddr) {
		DataServer::instance().addClient (connFd, clientAddr);
	});
}

int
main (void)
{
	signal (SIGPIPE, SIG_IGN);
	std::cout << "FreeFTP-Server v1.0\n";
	std::thread ctrlThread = std::thread ([] { solveConrtrolConnection(); });
	// std::thread dataThread = std::thread ([] { solveDataConnection(); });
	ctrlThread.join();
	// dataThread.join();
	return 0;
}

#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "BaseUtil.hpp"
#include <memory>
#include "EasySelect.hpp"
#include <thread>
#include <sstream>
#include <mutex>

std::thread serveThread;

void serveForClient (int connfd, struct sockaddr_in clienAddr)
{
	static EasySelect selector;
	static std::mutex stopStartMutex;
	static std::thread serveThread = std::thread ([&]
	{
		while (true)
		{
			stopStartMutex.lock();
			stopStartMutex.unlock();
			
			selector.startSelect();
		}
	});
	
	std::string acceptMessage = "220 FreeFTP-Server v1.0\r\n";
	IOUtil::writen (connfd, acceptMessage.c_str(), acceptMessage.length());
	
	stopStartMutex.lock();
	selector.addFd ({connfd, [] (int fd)
	{
	
		char buf[4096];
		while (true)
		{
			int nRead = read (fd, buf, sizeof (buf));
			if (nRead == 0)
			{
				close (fd);
				selector.removeFd (fd);
			}
			else if (nRead < 0)
			{
				close (fd);
				selector.removeFd (fd);
				perror ("read()");
			}
			else
			{
				std::string userCmd = "USER";
				std::string passCmd = "PASS";
				
				std::stringstream stream;
				stream.write (buf, nRead);
                std::string request = stream.str();
				std::cerr << request << "\n";
				if (request.compare (0, userCmd.length(), userCmd) == 0)
				{
					std::string reply = "331 Password,please.\r\n";
					IOUtil::writen (fd, reply.c_str(), reply.length());
					std::cerr << reply;
				}
				else if (request.compare (0, passCmd.length(), passCmd) == 0)
				{
					std::string reply = "230 Login Ok.\r\n";
					IOUtil::writen (fd, reply.c_str(), reply.length());
					std::cerr << reply;
				}
				
				IOUtil::writen (STDOUT_FILENO, buf, nRead);
				
			}
		}
		
	}});
	stopStartMutex.unlock();
}

void start()
{
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons (21);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	
	int listenFd = socket (AF_INET, SOCK_STREAM, 0);
	if (listenFd == -1)
	{
		perror ("socket()");
		return;
	}
	
	if (bind (listenFd, (struct sockaddr *) &servAddr, sizeof (servAddr)) == -1)
	{
		close (listenFd);
		perror ("bind()");
		return;
	}
	constexpr int LISTEN_QUEUE_SIZE = 10;
	if (listen (listenFd, LISTEN_QUEUE_SIZE) == -1)
	{
		close (listenFd);
		perror ("listen()");
		return;
	}
	
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	while (1)
	{
		int connfd = accept (listenFd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (connfd == -1)
		{
			perror ("accept()");
			continue;
		}
		else
		{
			serveForClient (connfd, clientAddr);
		}
	}
}

int main (void)
{
	std::cout << "FreeFTP-Server v1.0\n";
	start();
}

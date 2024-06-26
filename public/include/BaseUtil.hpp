#pragma once

#include "ErrorUtil.hpp"

#include <functional>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <vector>

struct NetUtil : public ErrorUtil {

	static int connectToServer (const std::string &domain, uint16_t port);
	static int waitForConnect (uint16_t port, struct sockaddr *peerAddr = nullptr, socklen_t *peerAddrLen = nullptr);
	static void syncLocalToRemote (
		int sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback = nullptr
	);
	static void syncRemoteToLocal (
		int sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback = nullptr
	);
	static std::string domainToIp (const std::string &domain);

	static void withPortBind (uint16_t port, std::function<void (int listenFd, struct sockaddr_in servAddr)> callback);

private:
	static void syncFile (int inFd, int outFd, std::function<void (size_t syncedSize)> callback = nullptr);
};

struct IOUtil : public ErrorUtil {
	static long getFileSize (const std::string &filePath);
	static long getFileSize (int fd);
	static size_t writen (int sockfd, const char *buf, size_t size);
	static size_t readn (int sockfd, char *buf, size_t size);
	static std::string readAll (int sockfd);
	static std::string getpass (const std::string &prompt, std::istream &iStream, std::ostream &oStream);
};
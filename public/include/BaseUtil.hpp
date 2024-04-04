#pragma once

#include "AbstractFd.hpp"
#include "ErrorUtil.hpp"

#include <functional>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <vector>

struct NetUtil : public ErrorUtil {

	static int connectToServer (const std::string &domain, uint16_t port);
	static int waitForConnect (
		uint16_t port, struct sockaddr *peerAddr = nullptr, socklen_t *peerAddrLen = nullptr
	);
	static void syncLocalToRemote (
		AbstractFd sockfd,
		const std::string &localPath,
		std::function<void (size_t syncedSize)> callback = nullptr
	);
	static void syncRemoteToLocal (
		AbstractFd sockfd,
		const std::string &localPath,
		std::function<void (size_t syncedSize)> callback = nullptr
	);
	static std::string domainToIp (const std::string &domain);

private:
	static void syncFile (
		AbstractFd inFd, AbstractFd outFd, std::function<void (size_t syncedSize)> callback = nullptr
	);
};

struct IOUtil : public ErrorUtil {
	static long getFileSize (const std::string &filePath);
	static long getFileSize (AbstractFd fd);
	static size_t writen (AbstractFd sockfd, const char *buf, size_t size);
	static std::string readAll (AbstractFd sockfd);
};
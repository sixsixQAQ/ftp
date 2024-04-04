#define _XOPEN_SOURCE 700

#include "BaseUtil.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <numeric>
#include <sstream>
#include <string.h>
#include <unistd.h>

int
NetUtil::connectToServer (const std::string &domain, uint16_t port)
{
	int fd		   = -1;
	std::string ip = domainToIp (domain);
	if (ip == "") {
		return -1;
	}

	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port	= htons (port);

	if (inet_pton (AF_INET, ip.c_str(), &servAddr.sin_addr) == -1)
		goto FAILED;
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
		goto FAILED;
	if (connect (fd, (struct sockaddr *)&servAddr, sizeof (servAddr)) == -1)
		goto FAILED;

	goto SUCCESS;
SUCCESS:
	return fd;
FAILED:
	setError (strerror (errno));
	return -1;
}

int
NetUtil::waitForConnect (uint16_t port, struct sockaddr *peerAddr, socklen_t *peerAddrLen)
{
	int listenFd			   = -1;
	int connFd				   = -1;
	constexpr int LISTEN_QUEUE = 10;

	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family		 = AF_INET;
	addr.sin_port		 = htons (port);

	if ((listenFd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
		goto FAILED;
	if (bind (listenFd, (struct sockaddr *)&addr, sizeof (addr)) == -1)
		goto FAILED;
	if (listen (listenFd, LISTEN_QUEUE) == -1)
		goto FAILED;
	if ((connFd = accept (listenFd, peerAddr, peerAddrLen)) == -1)
		goto FAILED;

	goto SUCCESS;
SUCCESS:
	close (listenFd);
	return connFd;

FAILED:
	if (listenFd != -1)
		close (listenFd);
	setError (strerror (errno));
	return -1;
}

size_t
IOUtil::writen (AbstractFd sockfd, const char *buf, size_t size)
{
	size_t totalWrite = 0;
	for (; totalWrite < size;) {
		int nwrite = sockfd.write ((void *)(buf + totalWrite), size - totalWrite);
		if (nwrite < 0) {
			setError (strerror (errno));
			return totalWrite;
		}
		totalWrite += nwrite;
	}
	return totalWrite;
}

std::string
IOUtil::readAll (AbstractFd fd)
{
	std::stringstream stream;

	char buf[4096];
	for (;;) {
		int nRead = fd.read (buf, sizeof (buf));
		if (nRead == 0) {
			break;
		} else if (nRead < 0) {
			setError (strerror (errno));
			break;
		}
		stream.write (buf, nRead);
	}
	return stream.str();
}

std::string
NetUtil::domainToIp (const std::string &domain)
{
	char ipv4Buf[INET_ADDRSTRLEN];

	struct addrinfo hints;
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;

	struct addrinfo *ai_list;
	int ret = getaddrinfo (domain.c_str(), NULL, &hints, &ai_list);

	if (ret != 0) {
		setError (gai_strerror (ret));
		return "";
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)ai_list->ai_addr;

	if (inet_ntop (AF_INET, &addr->sin_addr, ipv4Buf, sizeof (ipv4Buf)) == NULL) {
		setError (strerror (errno));
		return "";
	}
	return ipv4Buf;
}

void
NetUtil::syncFile (AbstractFd inFd, AbstractFd outFd, std::function<void (size_t syncedSize)> callback)
{
	size_t syncedSize = 0;
	if (!inFd.isOpened() || !outFd.isOpened())
		return;
	char buf[4096];
	for (;;) {
		int nRead = inFd.read (buf, sizeof (buf));
		if (nRead < 0) {
			setError (strerror (errno));
			return;
		} else if (nRead == 0) {
			return;
		}

		int totalWrite = 0;
		for (; totalWrite < nRead;) {
			int nWrite = outFd.write (buf + totalWrite, nRead - totalWrite);
			if (nWrite < 0) {
				setError (strerror (errno));
				return;
			}
			totalWrite += nWrite;
			syncedSize += nWrite;
			if (callback)
				callback (syncedSize);
		}
	}
}

void
NetUtil::syncLocalToRemote (
	AbstractFd sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback
)
{
	AbstractFd localFd = AbstractFd (open (localPath.c_str(), O_RDONLY));
	if (!localFd.isOpened()) {
		setError (strerror (errno));
		return;
	}
	syncFile (localFd, sockfd, [&] (size_t syncedSize) { callback (syncedSize); });

	localFd.close();
}

void
NetUtil::syncRemoteToLocal (
	AbstractFd sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback
)
{

	AbstractFd localFd = AbstractFd (open (localPath.c_str(), O_WRONLY | O_CREAT, 0644));
	if (!localFd.isOpened()) {
		setError (strerror (errno));
		return;
	}
	syncFile (sockfd, localFd, [&] (size_t syncedSize) {
		if (callback)
			callback (syncedSize);
	});
	localFd.close();
}

long
IOUtil::getFileSize (AbstractFd fd)
{
	int64_t currPos;
	int64_t endPos;

	currPos = fd.lseek (0, AbstractFd::Whence::CUR);

	if (currPos == -1)
		goto FAILED;

	endPos = fd.lseek (0, AbstractFd::Whence::END);

	if (endPos == -1)
		goto FAILED;

	if (fd.lseek (currPos, AbstractFd::Whence::BEG) == -1)
		goto FAILED;
	return endPos;

FAILED:
	setError (strerror (errno));
	return -1;
}

long
IOUtil::getFileSize (const std::string &filePath)
{
	AbstractFd fd = AbstractFd (open (filePath.c_str(), O_RDONLY));
	if (!fd.isOpened()) {
		setError (strerror (errno));
		return -1;
	}
	long size = getFileSize (fd);
	fd.close();
	return size;
}

#define _XOPEN_SOURCE 700

#include "BaseUtil.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <numeric>
#include <sstream>
#include <string.h>
#include <termios.h>
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
IOUtil::writen (int sockfd, const char *buf, size_t size)
{
	size_t totalWrite = 0;
	for (; totalWrite < size;) {
		int nwrite = write (sockfd, buf + totalWrite, size - totalWrite);
		if (nwrite < 0) {
			setError (strerror (errno));
			return totalWrite;
		}
		totalWrite += nwrite;
	}
	return totalWrite;
}

size_t
IOUtil::readn (int sockfd, char *buf, size_t size)
{
	size_t totalRead = 0;
	for (; totalRead < size;) {
		int nRead = read (sockfd, buf + totalRead, size - totalRead);
		if (nRead < 0) {
			setError (strerror (errno));
			return totalRead;
		}
		totalRead += nRead;
	}
	return totalRead;
}

std::string
IOUtil::readAll (int fd)
{
	std::stringstream stream;

	char buf[4096];
	for (;;) {
		int nRead = read (fd, buf, sizeof (buf));
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
IOUtil::getpass (const std::string &prompt, std::istream &iStream, std::ostream &oStream)
{
	static char password[4096];

	oStream << prompt;

	struct termios oldt, newt;
	tcgetattr (STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ECHO;
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);

	iStream.getline(password, sizeof(password));

	newt.c_lflag |= ECHO;
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);

	return password;
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
NetUtil::withPortBind (uint16_t port, std::function<void (int listenFd, struct sockaddr_in servAddr)> callback)
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
	callback (listenFd, servAddr);
	::close (listenFd);
}

void
NetUtil::syncFile (int inFd, int outFd, std::function<void (size_t syncedSize)> callback)
{
	size_t syncedSize = 0;
	if (inFd < 0 || outFd < 0)
		return;
	char buf[4096];
	for (;;) {
		int nRead = read (inFd, buf, sizeof (buf));
		if (nRead < 0) {
			setError (strerror (errno));
			return;
		} else if (nRead == 0) {
			return;
		}

		int totalWrite = 0;
		for (; totalWrite < nRead;) {
			int nWrite = write (outFd, buf + totalWrite, nRead - totalWrite);
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
NetUtil::syncLocalToRemote (int sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback)
{
	int localFd = open (localPath.c_str(), O_RDONLY);
	if (localFd == -1) {
		setError (strerror (errno));
		return;
	}
	syncFile (localFd, sockfd, [&] (size_t syncedSize) {
		if (callback)
			callback (syncedSize);
	});

	close (localFd);
}

void
NetUtil::syncRemoteToLocal (int sockfd, const std::string &localPath, std::function<void (size_t syncedSize)> callback)
{
	int localFd = open (localPath.c_str(), O_WRONLY | O_CREAT, 0644);
	if (localFd == -1) {
		setError (strerror (errno));
		return;
	}
	syncFile (sockfd, localFd, [&] (size_t syncedSize) {
		if (callback)
			callback (syncedSize);
	});
	close (localFd);
}

long
IOUtil::getFileSize (int fd)
{

	long currPos;
	long endPos;

	currPos = lseek (fd, 0, SEEK_CUR);

	if (currPos == -1)
		goto FAILED;

	endPos = lseek (fd, 0, SEEK_END);

	if (endPos == -1)
		goto FAILED;

	if (lseek (fd, 0, currPos) == -1)
		goto FAILED;
	return endPos;

FAILED:
	setError (strerror (errno));
	return -1;
}

long
IOUtil::getFileSize (const std::string &filePath)
{
	int fd = open (filePath.c_str(), O_RDONLY);
	if (fd == -1) {
		setError (strerror (errno));
		return -1;
	}
	long size = getFileSize (fd);
	close (fd);
	return size;
}

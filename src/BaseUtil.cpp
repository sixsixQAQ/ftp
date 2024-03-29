#define _XOPEN_SOURCE 700

#include "BaseUtil.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int NetUtil::connectToServer(const std::string &domain, uint16_t port)
{
    int fd = -1;
    std::string ip = domainToIp(domain);
    if (ip == "")
    {
        return -1;
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &servAddr.sin_addr) == -1)
        goto FAILED;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        goto FAILED;
    if (connect(fd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
        goto FAILED;

    goto SUCCESS;
SUCCESS:
    return fd;
FAILED:
    setError(strerror(errno));
    return -1;
}

int NetUtil::waitForConnect(uint16_t port, struct sockaddr *peerAddr, socklen_t *peerAddrLen)
{
    int listenFd = -1;
    int connFd = -1;
    constexpr int LISTEN_QUEUE = 10;

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        goto FAILED;
    if (bind(listenFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        goto FAILED;
    if (listen(listenFd, LISTEN_QUEUE) == -1)
        goto FAILED;
    if ((connFd = accept(listenFd, peerAddr, peerAddrLen)) == -1)
        goto FAILED;

    goto SUCCESS;
SUCCESS:
    close(listenFd);
    return connFd;

FAILED:
    if (listenFd != -1)
        close(listenFd);
    setError(strerror(errno));
    return -1;
}

int NetUtil::writen(int sockfd, const char *buf, size_t size)
{
    int totalWrite = 0;
    for (;;)
    {
        int nwrite = write(sockfd, buf + totalWrite, size - totalWrite);
        if (nwrite < 0)
        {
            setError(strerror(errno));
            return totalWrite;
        }
        totalWrite += nwrite;
    }
    return totalWrite;
}

std::string NetUtil::domainToIp(const std::string &domain)
{
    char ipv4Buf[INET_ADDRSTRLEN];

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    struct addrinfo *ai_list;
    int ret = getaddrinfo(domain.c_str(), NULL, &hints, &ai_list);

    if (ret != 0)
    {
        setError(gai_strerror(ret));
        return "";
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)ai_list->ai_addr;

    if (inet_ntop(AF_INET, &addr->sin_addr, ipv4Buf, sizeof(ipv4Buf)) == NULL)
    {
        setError(strerror(errno));
        return "";
    }
    return ipv4Buf;
}

std::string FTPUtil::makeUpCmd(const std::vector<std::string> &args)
{
    std::string command;
    for (auto arg : args)
    {
        command = command + ' ' + arg;
    }
    command += "\r\n";
    return command;
}

bool FTPUtil::sendCmd(ControlFd sockfd, const std::vector<std::string> &args)
{
    std::string command = makeUpCmd(args);

    size_t nwrite = NetUtil::writen(sockfd, command.c_str(), command.length());
    if (nwrite == command.length())
        return true;
    return false;
}
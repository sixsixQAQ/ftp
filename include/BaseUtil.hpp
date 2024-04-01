#pragma once

#include <functional>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

#include <sys/socket.h>

#include "Context.hpp"
#include "ErrorUtil.hpp"

struct NetUtil : public ErrorUtil
{

    static int connectToServer(const std::string &domain, uint16_t port);
    static int waitForConnect(uint16_t port, struct sockaddr *peerAddr = nullptr, socklen_t *peerAddrLen = nullptr);
    static size_t writen(int sockfd, const char *buf, size_t size);
    static std::string readAll(int sockfd);
    static std::string domainToIp(const std::string &domain);
};

struct FTPUtil
{
    using ArgList = std::vector<std::string>;
    static bool sendCmd(ControlFd sockfd, const ArgList &args);
    static bool sendCmd(ControlFd sockfd, std::function<ArgList(std::istream &)> parser, std::istream &inStream);

  private:
    static std::string makeUpCmd(const std::vector<std::string> &args);
};
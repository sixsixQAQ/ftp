#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <functional>

#include <sys/socket.h>

#include "Context.hpp"
#include "ErrorUtil.hpp"

struct NetUtil : public ErrorUtil
{

    static int connectToServer(const std::string &domain, uint16_t port);
    static int waitForConnect(uint16_t port, struct sockaddr *peerAddr = nullptr, socklen_t *peerAddrLen = nullptr);
    static int writen(int sockfd, const char *buf, size_t size);
    static void readAll(std::function<void(const char *buf, size_t size)> callback);
    static std::string domainToIp(const std::string &domain);
};

struct FTPUtil
{
    static bool sendCmd(ControlFd sockfd, const std::vector<std::string> &args);

  private:
    static std::string makeUpCmd(const std::vector<std::string> &args);
};
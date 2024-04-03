#pragma once

#include <vector>
#include <functional>
#include "Context.hpp"

struct FTPUtil
{
    using ArgList = std::vector<std::string>;
    static bool sendCmd(ControlFd sockfd, const ArgList &args);
    static bool sendCmd(ControlFd sockfd, std::function<ArgList(std::istream &)> parser, std::istream &inStream);

  private:
    static std::string makeUpCmd(const std::vector<std::string> &args);
};
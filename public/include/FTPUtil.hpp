#pragma once

#include "AbstractFd.hpp"

#include <functional>
#include <vector>

struct FTPUtil {
	using ArgList = std::vector<std::string>;
	static bool sendCmd (ControlFd sockfd, const ArgList &args);
	static bool sendCmd (ControlFd sockfd, std::function<ArgList (std::istream &)> parser, std::istream &inStream);

private:
	static std::string makeUpCmd (const std::vector<std::string> &args);
};
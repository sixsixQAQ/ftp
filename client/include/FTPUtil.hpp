#pragma once

#include "Context.hpp"

#include <functional>
#include <vector>

struct FTPUtil {
	using ArgList = std::vector<std::string>;
	static bool sendCmd (AbstractFd sockfd, const ArgList &args);
	static bool sendCmd (
		AbstractFd sockfd, std::function<ArgList (std::istream &)> parser, std::istream &inStream
	);

private:
	static std::string makeUpCmd (const std::vector<std::string> &args);
};
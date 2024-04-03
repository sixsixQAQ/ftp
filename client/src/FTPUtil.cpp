#include "FTPUtil.hpp"
#include "BaseUtil.hpp"
#include <numeric>

std::string FTPUtil::makeUpCmd (const std::vector<std::string> &args)
{
	std::string command;
	command = std::accumulate (args.begin(), args.end(), std::string(),
	[] (const std::string &a, const std::string &b) { return a.empty() ? b : a + ' ' + b; });
	command += "\r\n";
	return command;
}

bool FTPUtil::sendCmd (ControlFd sockfd, const ArgList &args)
{
	std::string command = makeUpCmd (args);
	
	size_t nwrite = IOUtil::writen (sockfd, command.c_str(), command.length());
	if (nwrite == command.length())
		return true;
	return false;
}

bool FTPUtil::sendCmd (ControlFd sockfd, std::function<ArgList (std::istream &) > parser, std::istream &inStream)
{
	return sendCmd (sockfd, parser (inStream));
}
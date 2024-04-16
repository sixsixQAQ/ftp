#include "StrUtil.hpp"

#include <sstream>

std::vector<std::string>
StrUtil::splitCmdLine (const std::string &str)
{
	std::vector<std::string> tokens;
	std::stringstream stream (str);

	std::string token;
	while (stream >> token) {
		tokens.emplace_back (token);
	}
	return tokens;
}

std::vector<std::string>
StrUtil::split (const std::string &text, const std::string &delimStr)
{
	std::vector<std::string> tokens;

	size_t nextLineBeg = 0;
	while (true) {
		size_t nextDelim = text.find (delimStr, nextLineBeg);
		if (nextDelim == std::string::npos) // no more delim
		{
			tokens.emplace_back (text.substr (nextLineBeg));
			break;
		} else {
			tokens.emplace_back (text.substr (nextLineBeg, nextDelim - nextLineBeg + delimStr.length()));
			nextLineBeg = nextDelim + delimStr.length();
		}
	}
	return tokens;
}
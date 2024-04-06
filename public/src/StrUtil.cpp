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
#pragma once

#include <string>
#include <vector>

struct StrUtil {
	static std::vector<std::string> splitCmdLine (const std::string &str);
	static std::vector<std::string> split (const std::string &text, const std::string &delimStr);
};
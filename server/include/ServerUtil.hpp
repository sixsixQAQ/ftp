#pragma once

#include "BackableReader.hpp"
#include "ErrorUtil.hpp"

#include <vector>

struct RequestUtil : public ErrorUtil {

	static std::string parseOneFullRequest (BackableReader &reader, const char *buf, size_t size);
	static std::vector<std::string> parseOneFullRequest_v2 (BackableReader &reader, const char *buf, size_t size);
};
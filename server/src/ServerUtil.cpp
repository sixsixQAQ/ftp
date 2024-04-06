#include "ServerUtil.hpp"

#include "StrUtil.hpp"

std::string
RequestUtil::parseOneFullRequest (BackableReader &reader, const char *buf, size_t size)
{
	std::string str (buf, size);
	size_t pos = str.find ("\r\n");
	if (pos == std::string::npos) {
		reader.unread ((void *)buf, size);
		return "";
	} else {
		size_t len		= pos + 2;
		std::string ret = str.substr (0, len);
		reader.unread ((void *)(buf + len), size - len);
		return ret;
	}
}

std::vector<std::string>
RequestUtil::parseOneFullRequest_v2 (BackableReader &reader, const char *buf, size_t size)
{
	return StrUtil::splitCmdLine (parseOneFullRequest (reader, buf, size));
}

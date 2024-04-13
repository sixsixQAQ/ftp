#include "ErrorUtil.hpp"

#include <string>

struct SysUtil : public ErrorUtil {
	static bool authenticate (const std::string &username, const std::string &password);
	static std::string getHomeOf (const std::string &username);
	static std::string listDir (const std::string &path);
};
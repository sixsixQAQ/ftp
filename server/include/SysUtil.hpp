#include "ErrorUtil.hpp"

#include <string>

struct SysUtil : public ErrorUtil {
	static bool authenticate (const std::string &username, const std::string &password);
	static std::string getHomeOf (const std::string &username);
	static std::string listDir (const std::string &path);
	static std::string listDirNameOnly (const std::string &path);
	static bool cdup (std::string &currDir);
	static bool cd (std::string &currDir, const std::string &path);
private:
	static std::string getShellResult (const std::string &cmd);
};
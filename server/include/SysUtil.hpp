#include "ErrorUtil.hpp"

#include <string>

struct SysUtil : public ErrorUtil {
	static bool authenticate (const std::string &username, const std::string &password);
	static std::string getHomeOf (const std::string &username);
	static std::string listDir (const std::string &path);
	static std::string listDirNameOnly (const std::string &path);
	static bool cdup (std::string &currDir);
	static bool cd (std::string &currDir, const std::string &path);
	static std::string realPath (const std::string &path);
	static std::string absolutePath (const std::string &currDir, const std::string &path);
	static bool rename (const std::string &, const std::string &);
	static std::string realAbsolutePath (const std::string &currDir, const std::string &path);
	static bool createDir (const std::string &path);
	static bool removeDir (const std::string &path);
	static bool removeFile (const std::string &path);
private:
	static std::string getShellResult (const std::string &cmd);
};
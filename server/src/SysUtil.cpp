#include "SysUtil.hpp"

#include <cstring>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <sys/types.h>
#include <unistd.h>

/// 出处:
/// https://stackoverflow.com/questions/17499163/how-to-check-password-in-linux-by-using-c-or-shell
bool
SysUtil::authenticate (const std::string &username, const std::string &passwd)
{

	struct passwd *passwdEntry = getpwnam (username.c_str());
	if (!passwdEntry) {
		// printf ("User '%s' doesn't exist\n", user);
		return 1;
	}
	// password is in /etc/passwd
	if (strcmp (passwdEntry->pw_passwd, "x") != 0) {
		return strcmp (passwdEntry->pw_passwd, crypt (passwd.c_str(), passwdEntry->pw_passwd)) == 0;
	} else {
		// password is in /etc/shadow
		struct spwd *shadowEntry = getspnam (username.c_str());
		if (!shadowEntry) {
			// printf ("Failed to read shadow entry for user '%s'\n", user);
			return false;
		}

		return strcmp (shadowEntry->sp_pwdp, crypt (passwd.c_str(), shadowEntry->sp_pwdp)) == 0;
	}
}

std::string
SysUtil::getHomeOf (const std::string &username)
{
	// TODO:换成线程安全的。
	struct passwd *pw = getpwnam (username.c_str());
	if (pw == nullptr) {
		return "";
	}
	return pw->pw_dir;
}

std::string
SysUtil::listDir (const std::string &path)
{
	std::string cmd = "ls -l " + path;
	FILE *inStream	= popen (cmd.c_str(), "r");
	if (inStream == nullptr) {
		return "";
	}

	std::string ret;
	char buf[4096];
	while (1) {
		if (fgets (buf, sizeof (buf), inStream) == NULL) {
			break;
		} else
			ret += buf;
	}
	pclose (inStream);
	return ret;
}

bool
SysUtil::cdup (std::string &currDir)
{
	// char buf[PATH_MAX];
	// if (realpath (currDir.c_str(), buf) == nullptr) {
	// 	return false;
	// }
	// currDir = buf;
	// return true;
	return SysUtil::cd (currDir, "..");
}

bool
SysUtil::cd (std::string &currDir, const std::string &path)
{
	if (path.size() == 0)
		return true;
	std::string fullPath;
	if (path[0] == '/') {
		fullPath = path;
	} else {
		fullPath = currDir + "/" + path;
	}

	char buf[PATH_MAX];
	if (realpath (fullPath.c_str(), buf) == nullptr) {
		return false;
	}
	currDir = buf;
	return true;
}

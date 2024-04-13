#include "SysUtil.hpp"

#include <cstring>
#include <pwd.h>
#include <shadow.h>
#include <sys/types.h>
#include <unistd.h>

// 验证用户登录信息
bool
SysUtil::authenticate (const std::string &username, const std::string &password)
{
	struct passwd *sp;
	sp = getpwnam (username.c_str());
	if (sp != NULL) {
		// 比较用户提供的密码与系统中存储的密码哈希值
		return strcmp (crypt (password.c_str(), sp->pw_passwd), sp->pw_passwd) == 0;
	}
	return false;
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
	FILE *inStream = popen ("ls -l", "r");
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

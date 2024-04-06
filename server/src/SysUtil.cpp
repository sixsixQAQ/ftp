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
	struct spwd *sp;
	sp = getspnam (username.c_str());
	if (sp != NULL) {
		// 比较用户提供的密码与系统中存储的密码哈希值
		return strcmp (crypt (password.c_str(), sp->sp_pwdp), sp->sp_pwdp) == 0;
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
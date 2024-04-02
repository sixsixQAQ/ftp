#pragma once
#include <string>
#include <vector>

#include "Context.hpp"

ControlFd openImpl(const std::string &domain, uint16_t port);
void _userImpl_(const ControlFd &fd, const std::string &userName);
void _passwordImpl_(const ControlFd &fd, const std::string &passwd);

void passiveImpl(Toggle &mode);

void pwdImpl(const ControlFd &fd);
void deleteImpl(const ControlFd &fd, const std::string &filePath);
void rmdirImpl(const ControlFd &fd, const std::string &dirPath);
void mkdirImpl(const ControlFd &fd, const std::string &dirPath);
std::string lsImpl(const ControlFd &fd, const Toggle &isPassive, const std::string &path = std::string());
void renameImpl(const ControlFd &fd, const std::string &oldname, const std::string &newname);
void putImpl(const ControlFd &fd, const std::string &localPath, const std::string &remotePath);
void getImpl(const ControlFd &fd, const std::string &remotePath, const std::string &localPath);
void cdImpl(const ControlFd &fd, const std::string &path);
[[noreturn]] void quitImpl(int connFd);
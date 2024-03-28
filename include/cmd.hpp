#pragma once
#include <string>
#include <vector>

#include "Context.hpp"

ControlFd openImpl(const std::string &domain, uint16_t port);
void passiveImpl(Toggle &mode);

void pwdImpl(const ControlFd &fd);
void deleteImpl(const ControlFd &fd, const std::string &filePath);
void rmdirImpl(const ControlFd &fd, const std::string &dirPath);
void mkdirImpl(const ControlFd &fd, const std::string &dirPath);
void lsImpl(const ControlFd &fd, const Toggle &isPassive, const std::string &path = std::string());

[[noreturn]] void quitImpl(int connFd);
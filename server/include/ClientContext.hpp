#pragma once

#include "AbstractFd.hpp"
#include "Toggle.hpp"

#include <netinet/in.h>
#include <string>

struct ClientContext {
	ControlFd ctrlFd = -1;
	DataFd dataFd	 = -1;
	Toggle PASVToggle;
	struct sockaddr_in clientAddr;

	std::string username;
	std::string password;
	std::string currDir;
	bool isLogined = false;
};
#pragma once

#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "Toggle.hpp"

#include <netinet/in.h>
#include <string>

struct ClientContext {
	ControlFd ctrlFd = -1;
	DataFd dataFd	 = -1;
	BackableReader ctrlReader;
	Toggle PASVToggle;
	struct sockaddr_in clientAddr;

	std::string username;
	std::string password;
	std::string currDir;
	bool isLogined = false;
};
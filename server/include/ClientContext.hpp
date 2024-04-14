#pragma once

#include "AbstractFd.hpp"
#include "BackableReader.hpp"
#include "Toggle.hpp"

#include <netinet/in.h>
#include <string>

enum class RepresentationType { ASCII, BINARY };

struct ClientContext {
	ControlFd ctrlFd = -1;
	DataFd dataFd	 = -1;
	BackableReader ctrlReader;
	Toggle PASVToggle;
	struct sockaddr_in clientAddr;
	enum RepresentationType representationType = RepresentationType::ASCII;
	std::string username;
	std::string password;
	std::string currDir;
	bool isLogined = false;
};
#pragma once

#include "ClientCmd.hpp"

#include <exception>

class quitException : public std::exception {
public:
	using exception::exception;
};

class quitCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	bool isControlConnectionNeeded () override;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;

	std::string help () override;

	void exec () override;
};
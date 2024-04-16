#pragma once

#include "ClientCmd.hpp"

class helpCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	bool isControlConnectionNeeded () override;

	std::string help () override;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;

	void exec () override;
};
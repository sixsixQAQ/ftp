#pragma once

#include "ClientCmd.hpp"

class quitCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	bool isControlConnectionNeeded () override;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;

	std::string help () override;

	void exec () override;
};
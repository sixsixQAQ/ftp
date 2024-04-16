#pragma once

#include "ClientCmd.hpp"

class getCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	bool isDataConnectionNeeded () override;

	std::string help () override;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;

	void exec () override;
};
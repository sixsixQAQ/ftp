#pragma once

#include "ClientCmd.hpp"

class rhelpCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	std::string help () override;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;

	void exec () override;
};
#pragma once

#include "ClientCmd.hpp"

class pwdCmd : public ClientCmd {
public:
	using ClientCmd::ClientCmd;

	std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () override;


	std::string help () override;

	void exec () override;
};
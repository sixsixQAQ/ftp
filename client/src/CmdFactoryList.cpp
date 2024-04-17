#include "ClientCmd/ClientCmd.hpp"
#include "ClientCmd/cdCmd.hpp"
#include "ClientCmd/deleteCmd.hpp"
#include "ClientCmd/getCmd.hpp"
#include "ClientCmd/helpCmd.hpp"
#include "ClientCmd/lsCmd.hpp"
#include "ClientCmd/mkdirCmd.hpp"
#include "ClientCmd/openCmd.hpp"
#include "ClientCmd/passiveCmd.hpp"
#include "ClientCmd/putCmd.hpp"
#include "ClientCmd/pwdCmd.hpp"
#include "ClientCmd/quitCmd.hpp"
#include "ClientCmd/renameCmd.hpp"
#include "ClientCmd/rhelpCmd.hpp"
#include "ClientCmd/rmdirCmd.hpp"
#include "ClientCmd/sizeCmd.hpp"
#include "ClientCmd/typeCmd.hpp"

#define FACTORY_PAIR(name)                                                                                             \
	{                                                                                                                  \
		#name, [](Context & c, const ArgList &a) { return std::make_shared<name##Cmd> (c, a); }                        \
	}

const std::vector<std::pair<std::string, ClientCmd::Factory>> &
ClientCmd::getFactoryList()
{
	static std::vector<std::pair<std::string, Factory>> factoryList = {
		FACTORY_PAIR (cd),
		FACTORY_PAIR (delete),
		FACTORY_PAIR (get),
		FACTORY_PAIR (help),
		FACTORY_PAIR (ls),
		FACTORY_PAIR (mkdir),
		FACTORY_PAIR (open),
		FACTORY_PAIR (passive),
		FACTORY_PAIR (put),
		FACTORY_PAIR (pwd),
		FACTORY_PAIR (quit),
		FACTORY_PAIR (rename),
		FACTORY_PAIR (rhelp),
		FACTORY_PAIR (rmdir),
		FACTORY_PAIR (size),
		FACTORY_PAIR (type),
	};
	return factoryList;
}
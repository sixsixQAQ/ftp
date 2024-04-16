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

#include <map>

const std::map<std::string, ClientCmd::Factory> &
ClientCmd::getFactoryMap()
{
	static std::map<std::string, Factory> factoryMap = {
		{"open", [] (Context &c, const ArgList &a) { return std::make_shared<openCmd> (c, a); }},
		{"passive", [] (Context &c, const ArgList &a) { return std::make_shared<passiveCmd> (c, a); }},
		{"ls", [] (Context &c, const ArgList &a) { return std::make_shared<lsCmd> (c, a); }},
		{"pwd", [] (Context &c, const ArgList &a) { return std::make_shared<pwdCmd> (c, a); }},
		{"cd", [] (Context &c, const ArgList &a) { return std::make_shared<cdCmd> (c, a); }},
		{"delete", [] (Context &c, const ArgList &a) { return std::make_shared<deleteCmd> (c, a); }},
		{"mkdir", [] (Context &c, const ArgList &a) { return std::make_shared<mkdirCmd> (c, a); }},
		{"rename", [] (Context &c, const ArgList &a) { return std::make_shared<renameCmd> (c, a); }},
		{"get", [] (Context &c, const ArgList &a) { return std::make_shared<getCmd> (c, a); }},
		{"put", [] (Context &c, const ArgList &a) { return std::make_shared<putCmd> (c, a); }},
		{"quit", [] (Context &c, const ArgList &a) { return std::make_shared<quitCmd> (c, a); }},
		{"size", [] (Context &c, const ArgList &a) { return std::make_shared<sizeCmd> (c, a); }},
		{"rmdir", [] (Context &c, const ArgList &a) { return std::make_shared<rmdirCmd> (c, a); }},
		{"help", [] (Context &c, const ArgList &a) { return std::make_shared<helpCmd> (c, a); }},
		{"rhelp", [] (Context &c, const ArgList &a) { return std::make_shared<rhelpCmd> (c, a); }},
		{"type", [] (Context &c, const ArgList &a) { return std::make_shared<typeCmd> (c, a); }},
	};
	return factoryMap;
}
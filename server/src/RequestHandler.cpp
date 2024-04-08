#include "RequestHandler.hpp"

#include "FTPUtil.hpp"
#include "SysUtil.hpp"

#include <functional>
#include <unordered_map>

struct Handlers {
	using Handler = std::function<void (ClientContext &, const std::vector<std::string>)>;

	static void USER_handler (ClientContext &context, const std::vector<std::string> args);
	static void PASS_handler (ClientContext &context, const std::vector<std::string> args);
	// static void LIST_handler(ClientContext &context, const std::vector<std::string> args);
	static void PWD_handler (ClientContext &context, const std::vector<std::string> args);
	static void QUIT_handler (ClientContext &context, const std::vector<std::string> args);
	static void PASV_handler (ClientContext &context, const std::vector<std::string> args);

	static std::unordered_map<std::string, Handler> &getHandlerMap ()
	{
		static std::unordered_map<std::string, Handler> handlerMap {
			{"USER", USER_handler},
			{"PASS", PASS_handler},
			{"PWD", PWD_handler},
			{"QUIT", QUIT_handler},
			{"PASV", PASV_handler},
		};
		return handlerMap;
	}
};

RequestHandler::RequestHandler (ClientContext context) : m_context (context)
{
}

void
RequestHandler::exec (std::vector<std::string> requestArgs)
{
	if (requestArgs.empty())
		return;
	auto handlerMap = Handlers::getHandlerMap();
	auto pair		= handlerMap.find (requestArgs[0]);
	if (pair == handlerMap.end()) {
		FTPUtil::sendCmd (m_context.ctrlFd, {"502", "command is not implememnted"});
	} else {
		pair->second (m_context, requestArgs);
	}
}

void
Handlers::USER_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 2)
		return;
	context.username = args[1];
	FTPUtil::sendCmd (context.ctrlFd, {"331", "Please specify the password."});
}

void
Handlers::PASS_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 2)
		return;
	context.password = args[1];
	if (SysUtil::authenticate (context.username, context.password)) {
		FTPUtil::sendCmd (context.ctrlFd, {"230", "Login Ok"});

		context.isLogined = true;
		context.currDir	  = SysUtil::getHomeOf (context.username);
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"530", "Login incorrect."});
	}
}

void
Handlers::PWD_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 1)
		return;
	if (!context.isLogined) {
		// "530 Please login with USER and PASS.
		FTPUtil::sendCmd (context.ctrlFd, {"530", "Please login with USER and PASS."});
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"257", "\"" + context.currDir + "\" is the current directory"});
	}
}

void
Handlers::QUIT_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 1)
		return;
	if (context.isLogined) {
		FTPUtil::sendCmd (context.ctrlFd, {"221", "GoodBye."});
	}
}

void
Handlers::PASV_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 1)
		return;
	if (context.isLogined) {
		FTPUtil::sendCmd (context.ctrlFd, {"227", "Entering Passive Mode (139,199,176,107,159,126)"});
	}
}

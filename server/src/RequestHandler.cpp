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

	// static void PASV_handler(ClientContext &context, const std::vector<std::string> args);

	static std::unordered_map<std::string, Handler> &getHandlerMap ()
	{
		static std::unordered_map<std::string, Handler> handlerMap {
			{"USER", USER_handler},
			{"PASS", USER_handler},
			{"PWD", USER_handler},
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
		return;
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
	FTPUtil::sendCmd (context.ctrlFd, {"331", " ", "Please specify the password."});
}

void
Handlers::PASS_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 2)
		return;
	context.password = args[1];
	if (SysUtil::authenticate (context.username, context.password)) {
		FTPUtil::sendCmd (context.ctrlFd, {"230", " ", "Login Ok"});
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"530", " ", "Login incorrect."});
		context.isLogined = true;
		context.currDir	  = SysUtil::getHomeOf (context.username);
	}
}

void
Handlers::PWD_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (args.size() != 1)
		return;
	if (!context.isLogined) {
		// "530 Please login with USER and PASS.
		FTPUtil::sendCmd (context.ctrlFd, {"530", " ", "Please login with USER and PASS."});
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"257", " ", context.currDir});
	}
}

#include "RequestHandler.hpp"

#include "FTPUtil.hpp"
#include "Server.hpp"
#include "StrUtil.hpp"
#include "SysUtil.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <sstream>
#include <unordered_map>

struct Handlers {
	using Handler = std::function<void (ClientContext &, const std::vector<std::string>)>;

	static void USER_handler (ClientContext &context, const std::vector<std::string> args);
	static void PASS_handler (ClientContext &context, const std::vector<std::string> args);
	static void LIST_handler (ClientContext &context, const std::vector<std::string> args);
	static void PWD_handler (ClientContext &context, const std::vector<std::string> args);
	static void QUIT_handler (ClientContext &context, const std::vector<std::string> args);
	static void PASV_handler (ClientContext &context, const std::vector<std::string> args);
	static void TYPE_handler (ClientContext &context, const std::vector<std::string> args);
	static void CDUP_handler (ClientContext &context, const std::vector<std::string> args);
	static void CWD_handler (ClientContext &context, const std::vector<std::string> args);
	static void NLST_handler (ClientContext &context, const std::vector<std::string> args);
	static void RNFR_handler (ClientContext &context, const std::vector<std::string> args);
	static void RNTO_handler (ClientContext &context, const std::vector<std::string> args);
	static void MKD_handler (ClientContext &context, const std::vector<std::string> args);
	static void RMD_handler (ClientContext &context, const std::vector<std::string> args);
	static void DELE_handler (ClientContext &context, const std::vector<std::string> args);

	static std::unordered_map<std::string, Handler> &getHandlerMap ()
	{
		static std::unordered_map<std::string, Handler> handlerMap {
			{"USER", USER_handler},
			{"PASS", PASS_handler},
			{"PWD", PWD_handler},
			{"QUIT", QUIT_handler},
			{"PASV", PASV_handler},
			{"LIST", LIST_handler},
			{"TYPE", TYPE_handler},
			{"CDUP", CDUP_handler},
			{"CWD", CWD_handler},
			{"NLST", NLST_handler},
			{"RNFR", RNFR_handler},
			{"RNTO", RNTO_handler},
			{"MKD", MKD_handler},
			{"RMD", RMD_handler},
			{"DELE", DELE_handler},

		};
		return handlerMap;
	}
};

RequestHandler::RequestHandler (ClientContext &context) : m_context (context)
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
Handlers::NLST_handler (ClientContext &context, const std::vector<std::string> args)
{
	std::string result;
	if (args.size() == 1) {
		result = SysUtil::listDirNameOnly (context.currDir);
	} else if (args.size() == 2) {
		result = SysUtil::listDirNameOnly (SysUtil::realAbsoutePath (context.currDir, args[1]));
	} else {
		return;
	}
	//转成RFC959 3.4节要求的<CRLF>发送
	std::stringstream ss (result);
	std::vector<std::string> items;
	std::string buf;
	while (std::getline (ss, buf)) {
		items.emplace_back (buf);
	}
	if (items.begin() != items.end())
		items.erase (items.begin());

	FTPUtil::sendCmd (context.ctrlFd, {"150", "Here comes the directory listing."});

	std::for_each (items.begin(), items.end(), [&] (const std::string &item) {
		FTPUtil::sendCmd (context.dataFd.getFd(), {item});
	});

	context.dataFd.close();
	FTPUtil::sendCmd (context.ctrlFd, {"226", "Directory send OK."});
}

void
Handlers::RNFR_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Parameter error."});
		return;
	}
	context.RNFR_path = SysUtil::realAbsoutePath (context.currDir, args[1]);
	if (context.RNFR_path.empty()) {
		FTPUtil::sendCmd (context.ctrlFd, {"550", "File not exists."});
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"350", "File exists, ready for destination name."});
	}
}

void
Handlers::RNTO_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Parameter error."});
		return;
	}
	if (context.RNFR_path.empty()) {
		FTPUtil::sendCmd (context.ctrlFd, {"503", "RNFR required first."});
		return;
	}
	context.RNTO_path = SysUtil::absolutePath (context.currDir, args[1]);
	if (SysUtil::rename (context.RNFR_path, context.RNTO_path))
		FTPUtil::sendCmd (context.ctrlFd, {"250", "Rename successful."});
	else
		FTPUtil::sendCmd (context.ctrlFd, {"550", "Rename failed."});

	context.RNFR_path = context.RNTO_path = "";
}

void
Handlers::MKD_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Parameter error."});
		return;
	}
	if (SysUtil::createDir (SysUtil::absolutePath (context.currDir, args[1])))
		FTPUtil::sendCmd (
			context.ctrlFd, {"257", "\"" + SysUtil::realAbsoutePath (context.currDir, args[1]) + "\" created."}
		);
	else
		FTPUtil::sendCmd (context.ctrlFd, {"550", "Directory creation failed."});
}

void
Handlers::RMD_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Parameter error."});
		return;
	}
	const std::string realAbsPath = SysUtil::realAbsoutePath (context.currDir, args[1]);
	if (SysUtil::removeDir (realAbsPath))
		FTPUtil::sendCmd (context.ctrlFd, {"250", "Directory \"" + realAbsPath + "\" was removed."});
	else
		FTPUtil::sendCmd (context.ctrlFd, {"550", "Directory removal failed."});
}

void
Handlers::DELE_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Parameter error."});
		return;
	}
	const std::string realAbsPath = SysUtil::realAbsoutePath (context.currDir, args[1]);
	if (SysUtil::removeFile (realAbsPath))
		FTPUtil::sendCmd (context.ctrlFd, {"250", "File \"" + args[1] + "\" was removed."});
	else
		FTPUtil::sendCmd (context.ctrlFd, {"550", "File removal failed."});
}

void
Handlers::LIST_handler (ClientContext &context, const std::vector<std::string> args)
{
	std::string result;
	if (args.size() == 1) {
		result = SysUtil::listDir (context.currDir);
	} else if (args.size() == 2) {
		result = SysUtil::listDir (SysUtil::realAbsoutePath (context.currDir, args[1]));
	} else {
		return;
	}
	//转成RFC959 3.4节要求的<CRLF>发送
	std::stringstream ss (result);
	std::vector<std::string> items;
	std::string buf;
	while (std::getline (ss, buf)) {
		items.emplace_back (buf);
	}
	if (items.begin() != items.end())
		items.erase (items.begin());

	FTPUtil::sendCmd (context.ctrlFd, {"150", "Here comes the directory listing."});

	std::for_each (items.begin(), items.end(), [&] (const std::string &item) {
		FTPUtil::sendCmd (context.dataFd.getFd(), {item});
	});

	context.dataFd.close();
	FTPUtil::sendCmd (context.ctrlFd, {"226", "Directory send OK."});
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
		NetUtil::withPortBind (0, [&] (int listenFd, struct sockaddr_in unused) {
			(void)unused;

			struct sockaddr_in servAddr;
			socklen_t addrLen = sizeof (sockaddr_in);
			if (getsockname (listenFd, (struct sockaddr *)&servAddr, &addrLen) != 0) {
				return;
			}
			uint16_t port		 = ntohs (servAddr.sin_port);
			uint8_t headByte	 = ((uint8_t *)&port)[1];
			uint8_t tailByte	 = ((uint8_t *)&port)[0];
			std::string addrInfo = std::string ("Entering Passive Mode (139,199,176,107,") + std::to_string (headByte) +
								   "," + std::to_string (tailByte) + ").";
			FTPUtil::sendCmd (context.ctrlFd, {"227", addrInfo});
			std::thread acceptThread = std::thread ([&] {
				// TODO：这里应该加客户端信息检验
				int dataFd	   = accept (listenFd, nullptr, nullptr);
				context.dataFd = dataFd;
			});
			acceptThread.join();
		});

		// FTPUtil::sendCmd (context.ctrlFd, {"227", "Entering Passive Mode (139,199,176,107,255,253)."});
		// FTPUtil::sendCmd (context.ctrlFd, {"227", "Entering Passive Mode (127,0,0,1,255,253)."});
	}
}

void
Handlers::TYPE_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Unrecognised TYPE command."});
		return;
	}

	if (args[1] == "I") {
		FTPUtil::sendCmd (context.ctrlFd, {"200", "Switching to Binary mode."});
		context.representationType = RepresentationType::BINARY;
		return;
	} else if (args[1] == "A") {
		FTPUtil::sendCmd (context.ctrlFd, {"200", "Switching to ASCII mode."});
		context.representationType = RepresentationType::ASCII;
		return;
	} else {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Unrecognised TYPE command."});
	}
}

void
Handlers::CDUP_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 1) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Invalid parameter."});
		return;
	} else {
		if (SysUtil::cdup (context.currDir))
			FTPUtil::sendCmd (context.ctrlFd, {"250", "Directory changed to " + context.currDir});
		else
			FTPUtil::sendCmd (context.ctrlFd, {"550", "Failed to change directory."});
	}
}

void
Handlers::CWD_handler (ClientContext &context, const std::vector<std::string> args)
{
	if (!context.isLogined)
		return;
	if (args.size() != 2) {
		FTPUtil::sendCmd (context.ctrlFd, {"501", "Invalid parameter."});
	} else {
		if (SysUtil::cd (context.currDir, args[1]))
			FTPUtil::sendCmd (context.ctrlFd, {"250", "Directory changed to " + context.currDir});
		else
			FTPUtil::sendCmd (context.ctrlFd, {"550", "Failed to change directory."});
	}
}

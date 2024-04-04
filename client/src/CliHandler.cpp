#include "CliHandler.hpp"

#include "BaseUtil.hpp"
#include "ProtocolConfig.hpp"
#include "cmd.hpp"

#include <functional>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <vector>

class CmdHandler {
public:
	using Handler = std::function<void (Context &, const std::vector<std::string>)>;

	static void exec (Context &context, const std::vector<std::string> args);
	static void openHandler (Context &context, const std::vector<std::string> &args);
	static void passiveHandler (Context &context, const std::vector<std::string> &args);
	static void pwdHandler (Context &context, const std::vector<std::string> &args);
	static void deleteHandler (Context &context, const std::vector<std::string> &args);
	static void rmdirHandler (Context &context, const std::vector<std::string> &args);
	static void mkdirHandler (Context &context, const std::vector<std::string> &args);
	static void lsHandler (Context &context, const std::vector<std::string> &args);
	static void quitHandler (Context &context, const std::vector<std::string> &args);
	static void renameHandler (Context &context, const std::vector<std::string> &args);
	static void putHandler (Context &context, const std::vector<std::string> &args);
	static void getHandler (Context &context, const std::vector<std::string> &args);
	static void helpHandler (Context &context, const std::vector<std::string> &args);
	static void cdHandler (Context &context, const std::vector<std::string> &args);
	static void sizeHandler (Context &context, const std::vector<std::string> &args);

private:
	static bool checkConnection (Context &context);
	static void usage (Context &context, const std::string &name);

	struct CmdInfo {
		Handler handler;
		std::string usage;
		std::string help;
	};

	static const std::unordered_map<std::string, CmdInfo> &getHandlerMap ()
	{
		static std::unordered_map<std::string, CmdInfo> cmdHandlerMap {
			{"open",
			 {openHandler,
			  "usage: open host-name [port]",
			  "open\t\tconnect to remote ftp server"}},
			{"passive",
			 {passiveHandler,
			  "usage: passive",
			  "passive\t\ttoggle use of passive transfer mode"}},
			{"pwd",
			 {pwdHandler,
			  "usage: pwd",
			  "pwd\t\tprint working directory on remote machine"}},
			{"delete",
			 {deleteHandler,
			  "usage: delete remote-file",
			  "delete\t\tdelete remote file"}},
			{"rmdir",
			 {rmdirHandler,
			  "usage: rmdir directory-name",
			  "rmdir\t\tremove directory on the remote machine"}},
			{"mkdir",
			 {mkdirHandler,
			  "usage: mkdir directory-name",
			  "mkdir\t\tmake directory on the remote machine"}},
			{"ls", {lsHandler, "ls [remote-path]", "ls\t\tlist contents of remote path"}},
			{"quit",
			 {quitHandler, "usage: quit", "quit\t\tterminate ftp session and exit"}},
			{"rename",
			 {renameHandler, "usage: rename from-name to-name", "rename\t\trename file"}},
			{"put",
			 {putHandler, "usage: put local-file [remote-file]", "put\t\tsend one file"}},
			{"get",
			 {getHandler, "usage: get remote-file [local-file]", "get\t\treceive file"}},
			{"help",
			 {helpHandler,
			  "usage: help [cmd name]",
			  "help\t\tprint local help information"}},
			{"cd",
			 {cdHandler,
			  "usage: cd remote-directory",
			  "cd\t\tchange remote working directory"}},
			{"size",
			 {sizeHandler,
			  "usage: size remote-file",
			  "size\t\tshow size of remote file"}},

		};
		return cmdHandlerMap;
	}
};

static std::vector<std::string>
split (const std::string &str)
{
	std::vector<std::string> tokens;
	std::stringstream stream (str);

	std::string token;
	while (stream >> token) {
		tokens.emplace_back (token);
	}
	return tokens;
}

CliHandler::CliHandler (std::istream &inStream, std::ostream &outStream) :
	m_cliContext (inStream, outStream)
{
}

void
CliHandler::exec()
{
	std::string cmdLine;

	for (;;) {
		m_cliContext.outStream << "ftp>";
		std::vector<std::string> args = parseOneLine (std::cin);
		CmdHandler::exec (m_cliContext, args);
	}
}

std::vector<std::string>
CliHandler::parseOneLine (std::istream &inStream)
{
	std::string cmdLine;
	getline (inStream, cmdLine);
	return split (cmdLine);
}

void
CmdHandler::exec (Context &context, const std::vector<std::string> args)
{
	if (args.size() == 0)
		return;

	auto cmdPair = getHandlerMap().find (args[0]);

	if (cmdPair != getHandlerMap().end()) {
		cmdPair->second.handler (context, args);
	} else {
		context.outStream << "?Invalid command.\n";
	}
}

void
CmdHandler::openHandler (Context &context, const std::vector<std::string> &args)
{
	if (args.size() == 2) {
		context.ctrlFd = openImpl (args[1], SERVER_DEFAULT_CONTROL_CONN_PORT);
	} else if (args.size() == 3) {

		uint16_t port;
		try {
			port = std::stoul (args[2]);
			if (port < 1 || port > 65535)
				goto USAGE;
		} catch (const std::exception &) {
			goto USAGE;
		}

		context.ctrlFd = openImpl (args[1], std::stoul (args[2]));
		if (context.ctrlFd == -1) {
			context.outStream << "Failed to connect.\n";
			return;
		}
	} else {
		goto USAGE;
	}
	{
		context.outStream << "Username:";
		std::string userName;
		getline (context.inStream, userName);
		_userImpl_ (context.ctrlFd, userName);

		context.outStream << "Password:";
		std::string passwd;
		getline (context.inStream, passwd);
		_passwordImpl_ (context.ctrlFd, passwd);

		return;
	}

USAGE:
	usage (context, args[0]);
}

void
CmdHandler::passiveHandler (Context &context, const std::vector<std::string> &args)
{
	if (args.size() != 1) {
		usage (context, args[0]);
		return;
	}
	passiveImpl (context.PASV_Toggle);
	context.outStream << "Passive Mode: " << std::boolalpha << context.PASV_Toggle
					  << "\n";
}

void
CmdHandler::pwdHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 1) {
		usage (context, args[0]);
		return;
	}

	pwdImpl (context.ctrlFd);
}

void
CmdHandler::deleteHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 2) {
		usage (context, args[0]);
		return;
	}
	deleteImpl (context.ctrlFd, args[1]);
}

void
CmdHandler::rmdirHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 2) {
		usage (context, args[0]);
		return;
	}
	rmdirImpl (context.ctrlFd, args[1]);
}

void
CmdHandler::mkdirHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 2) {
		usage (context, args[0]);
		return;
	}
	mkdirImpl (context.ctrlFd, args[1]);
}

void
CmdHandler::usage (Context &context, const std::string &name)
{
	auto pair = getHandlerMap().find (name);
	if (pair == getHandlerMap().end())
		return;
	context.outStream << pair->second.usage;
}

bool
CmdHandler::checkConnection (Context &context)
{
	if (context.ctrlFd == -1) {
		context.outStream << "Not connected.\n";
		return false;
	}
	return true;
}

void
CmdHandler::lsHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() == 1) {
		context.outStream << lsImpl (context.ctrlFd, context.PASV_Toggle);
	} else if (args.size() == 2) {
		context.outStream << lsImpl (context.ctrlFd, context.PASV_Toggle, args[1]);
	} else {
		usage (context, args[0]);
	}
}

void
CmdHandler::quitHandler (Context &context, const std::vector<std::string> &args)
{
	if (args.size() != 1) {
		usage (context, args[0]);
	}
	quitImpl (context.ctrlFd);
}

void
CmdHandler::renameHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 3) {
		usage (context, args[0]);
		return;
	}
	renameImpl (context.ctrlFd, args[1], args[2]);
}

void
CmdHandler::putHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 3) {
		usage (context, args[0]);
		return;
	}

	const long wholeSize = IOUtil::getFileSize (args[1]);
	if (wholeSize == -1) {
		context.outStream << "No such file: " << args[1] << "\n";
		return;
	}

	putImpl (context.ctrlFd, args[1], args[2], [&] (size_t syncedSize) {
		auto str = "\rWhole Size:" + std::to_string (wholeSize / 1024.0 / 1024.0) +
				   "MB,transfered:" + std::to_string (syncedSize / 1024.0 / 1024.0) +
				   "MB";
		write (STDOUT_FILENO, str.c_str(), str.size());
	});
}

void
CmdHandler::getHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context))
		return;
	if (args.size() != 3) {
		usage (context, args[0]);
		return;
	}
	const long wholeSize = sizeImpl (context.ctrlFd, args[1]);
	if (wholeSize == -1) {
		context.outStream << "No such file: " << args[1] << "\n";
		return;
	}
	getImpl (context.ctrlFd, args[1], args[2], [&] (size_t syncedSize) {
		auto str = "\rWhole Size:" + std::to_string (wholeSize / 1024.0 / 1024.0) +
				   "MB,transfered:" + std::to_string (syncedSize / 1024.0 / 1024.0) +
				   "MB";
		write (STDOUT_FILENO, str.c_str(), str.size());
	});
}

void
CmdHandler::helpHandler (Context &context, const std::vector<std::string> &args)
{
	(void)(context);
	(void)(args);

	if (args.size() == 1) {
		int i = 1;
		for (auto pair : getHandlerMap()) {
			context.outStream << pair.first << "\t\t";
			if ((i++ % 5) == 0)
				context.outStream << "\n";
		}
	} else if (args.size() == 2) {
		auto pair = getHandlerMap().find (args[1]);
		if (pair == getHandlerMap().end()) {
			context.outStream << "?Invalid help command"
							  << "`" << args[1] << "`";
		} else {
			context.outStream << pair->second.help;
		}
	}
	context.outStream << "\n";
}

void
CmdHandler::cdHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context)) {
		return;
	}
	if (args.size() != 2) {
		usage (context, args[0]);
		return;
	}
	cdImpl (context.ctrlFd, args[1]);
}

void
CmdHandler::sizeHandler (Context &context, const std::vector<std::string> &args)
{
	if (!checkConnection (context)) {
		return;
	}
	if (args.size() != 2) {
		usage (context, args[0]);
		return;
	}
	long size = sizeImpl (context.ctrlFd, args[1]);

	if (size == -1) {
		context.outStream << "No such file: " << args[1] << "\n";
	} else {
		context.outStream << "File size: " << size << " byte.\n";
	}
}

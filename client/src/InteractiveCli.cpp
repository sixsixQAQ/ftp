#include "InteractiveCli.hpp"

#include "ClientCmd/ClientCmd.hpp"
#include "StrUtil.hpp"

static std::vector<std::string>
parseOneLine (std::istream &inStream)
{
	std::string cmdLine;
	getline (inStream, cmdLine);
	return StrUtil::splitCmdLine (cmdLine);
}

void
InteractiveCli::start (std::istream &inStream, std::ostream &outStream)
{
	outStream << "FreeFTP-Client v1.0\n";

	Context context (inStream, outStream);

	std::string cmdLine;

	for (;;) {
		context.outStream << "ftp> ";

		std::vector<std::string> args = parseOneLine (context.inStream);

		if (context.inStream.fail()) {
			break; //无输入，停止服务
		}

		if (args.empty()) {
			continue;
		}

		auto cmd = ClientCmd::cmdFactory (context, args);
		if (cmd) {
			cmd->templateExecution();
		} else {
			context.outStream << "Invalid command: " << args[0] << "\n";
		}
	}
}

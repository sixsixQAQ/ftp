#include "ClientCmd/ClientCmd.hpp"
#include "StrUtil.hpp"

std::vector<std::string>
parseOneLine (std::istream &inStream)
{
	std::string cmdLine;
	getline (inStream, cmdLine);
	return StrUtil::splitCmdLine (cmdLine);
}

int
main ()
{
	std::cout << "FreeFTP-Client v1.0\n";

	Context context (std::cin, std::cout);

	std::string cmdLine;

	for (;;) {
		context.outStream << "ftp>";
		std::vector<std::string> args = parseOneLine (std::cin);

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
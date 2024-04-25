#include "BaseUtil.hpp"
#include "ClientCmd/quitCmd.hpp"
#include "InteractiveCli.hpp"

#include <ext/stdio_filebuf.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	InteractiveCli cli;

	if (argc == 2) {
		if (argv[1] == std::string ("-gui")) {

			int fd;
			NetUtil::withPortBind (65535, [&] (int listenFd, struct sockaddr_in unused) {
				fd = accept (listenFd, nullptr, nullptr);
			});
			if (fd == -1) {
				perror ("accept()");
				return -1;
			}
			__gnu_cxx::stdio_filebuf<char> iFileBuf (fd, std::ios::in);
			__gnu_cxx::stdio_filebuf<char> oFileBuf (fd, std::ios::out);
			std::istream iStream (&iFileBuf);
			std::ostream oStream (&oFileBuf);

			oStream << std::unitbuf;
			try {
				cli.start (iStream, oStream);
			} catch (quitException &) {
			}
		}
	} else
		try {
			cli.start (std::cin, std::cout);
		} catch (quitException &) {
		}
	return 0;
}
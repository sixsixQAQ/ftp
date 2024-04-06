#include "CliHandler.hpp"

int
main ()
{
	std::cout << "FreeFTP-Client v1.0\n";
	CliHandler cliHandler (std::cin, std::cout);
	cliHandler.exec();
}
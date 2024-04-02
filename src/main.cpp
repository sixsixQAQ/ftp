#include "CliHandler.hpp"

int main()
{
    std::cout << "FreeFTP v1.0\n";
    CliHandler cliHandler(std::cin, std::cout);
    cliHandler.exec();
}
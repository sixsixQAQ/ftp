#include "CliHandler.hpp"

int main()
{
    CliHandler cliHandler(std::cin, std::cout);
    cliHandler.exec();
}
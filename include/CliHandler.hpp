#pragma once

#include "Context.hpp"
#include <iostream>
#include <string>
#include <vector>

class CliHandler
{
  public:
    // CliHandler& threadInstance();
    CliHandler(std::istream &inStream, std::ostream &outStream);
    void exec();
    static std::vector<std::string> parseOneLine(std::istream& inStream);
  private:
    Context m_cliContext;
};
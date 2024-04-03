#pragma once

#include <string>

class ErrorUtil
{
  public:
    static std::string getLastError();

  protected:
    static void setError(const std::string &error, const std::string &file = __builtin_FILE(),
                         const std::string &function = __builtin_FUNCTION(), int lineNo = __builtin_LINE());

  private:
    static thread_local std::string m_errorString;
    static thread_local std::string m_errorFunction;
    static thread_local std::string m_errorFile;
    static thread_local int m_errorLineNo;
};
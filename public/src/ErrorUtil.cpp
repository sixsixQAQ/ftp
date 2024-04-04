#include "ErrorUtil.hpp"

thread_local std::string ErrorUtil::m_errorFile;
thread_local std::string ErrorUtil::m_errorString;
thread_local std::string ErrorUtil::m_errorFunction;
thread_local int ErrorUtil::m_errorLineNo;

std::string
ErrorUtil::getLastError()
{
	return m_errorString + "," + m_errorFile + "," + m_errorFunction + "," +
		   std::to_string (m_errorLineNo);
}

void
ErrorUtil::setError (
	const std::string &error,
	const std::string &file,
	const std::string &function,
	int lineNo
)
{
	m_errorString	= error;
	m_errorFile		= file;
	m_errorFunction = function;
	m_errorLineNo	= lineNo;
}

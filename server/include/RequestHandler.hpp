#pragma once

#include "ClientContext.hpp"

#include <memory>
#include <string>
#include <vector>

class RequestHandler {
public:
	RequestHandler (ClientContext &context);
	void exec (std::vector<std::string> requestArgs);
private:
	ClientContext &m_context;
};
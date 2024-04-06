#pragma once

#include "EasySelect.hpp"

#include <memory>
#include <netinet/in.h>

class Server : public ErrorUtil {
public:
	Server (EasySelect *dispatcher, int ctrlFd, struct sockaddr_in addr);
	void notifyDataCome ();

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
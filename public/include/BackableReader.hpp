#pragma once

#include <memory>
#include <string>

class BackablaReader {
public:
	BackablaReader (int fd);
	int read (void *buf, size_t size);
	void rollBackData (void *buf, size_t size);
	std::string getCached ();
private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
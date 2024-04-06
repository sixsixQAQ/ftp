#pragma once

#include <memory>
#include <string>

class BackableReader {
public:
	BackableReader (int fd);
	int64_t read (void *buf, size_t size);
	void unread (void *buf, size_t size);
	std::string getCached ();

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
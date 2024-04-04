#pragma once

#include <deque>
#include <string>
#include <unistd.h>

class BackableReader {

public:
	BackableReader (int fd) : m_fd (fd)
	{
	}

	int read (void *buf, size_t size);
	void rollBackData (void *buf, size_t size);
	std::string getCached ();

private:
	int m_fd;
	std::string m_cache;
};

int
BackableReader::read (void *buf, size_t size)
{

	if (m_cache.empty()) {
		int nRead = ::read (m_fd, buf, size);

		return nRead;
	} else {
		m_cache.copy (static_cast<char *> (buf), size);
		if (size > m_cache.size()) {
			return m_cache.size();
		} else {
			m_cache = m_cache.substr (size);
			return size;
		}
	}
}

void
BackableReader::rollBackData (void *buf, size_t size)
{
}

std::string
BackableReader::getCached()
{
	return std::string();
}
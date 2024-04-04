#include "BackableReader.hpp"

#include <cassert>
#include <deque>
#include <unistd.h>

class BackablaReader::Impl {

public:
	Impl (int fd) : m_fd (fd)
	{
	}

	int read (void *buf, size_t size);
	void unread (void *buf, size_t size);
	std::string getCached ();

private:
	int m_fd;
	std::string m_cache;
};

int
BackablaReader::Impl::read (void *buf, size_t size)
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
BackablaReader::Impl::unread (void *buf, size_t size)
{
	std::string backed (static_cast<char *> (buf), size);
	m_cache = backed + m_cache;
}

std::string
BackablaReader::Impl::getCached()
{
	return m_cache;
}

BackablaReader::BackablaReader (int fd) : m_pImpl (std::make_shared<Impl> (fd))
{
	assert (m_pImpl);
}

int64_t
BackablaReader::read (void *buf, size_t size)
{
	assert (m_pImpl);
	return m_pImpl->read (buf, size);
}

void
BackablaReader::unread (void *buf, size_t size)
{
	assert (m_pImpl);
	return m_pImpl->unread (buf, size);
}

std::string
BackablaReader::getCached()
{
	assert (m_pImpl);
	return m_pImpl->getCached();
}

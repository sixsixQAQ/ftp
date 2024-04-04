#include "AbstractFd.hpp"

#include "BackableReader.hpp"

#include <cassert>
#include <unistd.h>

class AbstractFd::Impl {
public:

	Impl (int fd) : m_fd (fd), m_reader (fd)
	{
	}

	int getFd () const
	{
		return m_fd;
	}

	void close ()
	{
		::close (m_fd);
		m_fd = -1;
	}

	bool isOpened () const
	{
		return m_fd != -1;
	}

	int64_t read (void *buf, size_t size)
	{
		return m_reader.read (buf, size);
	}

	int64_t write (void *buf, size_t size)
	{
		return ::write (m_fd, buf, size);
	}

	void unread (void *buf, size_t size)
	{
		return m_reader.unread (buf, size);
	}

	static int toInt (Whence whence)
	{
		if (whence == Whence::CUR)
			return SEEK_CUR;
		else if (whence == Whence::BEG)
			return SEEK_SET;
		else if (whence == Whence::END) {
			return SEEK_END;
		}
		assert (false);
		return SEEK_CUR;
	}

	int64_t lseek (int64_t offset, Whence whence)
	{
		return ::lseek (m_fd, (off_t)offset, toInt (whence));
	}

private:
	int m_fd;
	BackablaReader m_reader;
};

AbstractFd::AbstractFd (int fd) : m_pImpl (std::make_shared<Impl> (fd))
{
	assert (m_pImpl);
}

void
AbstractFd::close()
{
	assert (m_pImpl);
	return m_pImpl->close();
}

ssize_t
AbstractFd::read (void *buf, size_t size)
{
	assert (m_pImpl);
	return m_pImpl->read (buf, size);
}

int64_t
AbstractFd::write (void *buf, size_t size)
{
	assert (m_pImpl);
	return m_pImpl->write (buf, size);
}

bool
AbstractFd::isOpened() const
{
	assert (m_pImpl);
	return m_pImpl->isOpened();
}

int64_t
AbstractFd::lseek (int64_t offset, Whence whence)
{
	assert (m_pImpl);
	return m_pImpl->lseek (offset, whence);
}

int
AbstractFd::getFd() const
{
	assert (m_pImpl);
	return m_pImpl->getFd();
}

void
BackableFd::unread (void *buf, size_t size)
{
	m_pImpl->unread (buf, size);
}

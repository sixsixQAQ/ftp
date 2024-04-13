#include "EasySelect.hpp"

#include "BaseUtil.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

class EasySelect::Impl {
public:
	Impl()
	{
		int pipeFds[2];
		if (pipe (pipeFds) == -1) {
			setError (strerror (errno));
			return;
		}
		m_pipeRead	= pipeFds[0];
		m_pipeWrite = pipeFds[1];
	}

	~Impl()
	{
		::close (m_pipeRead);
		::close (m_pipeWrite);
	}

	std::vector<int> waitForReadAble ();
	void addFd (int fd);
	void removeFd (int fd);
	void stop ();
	void waitForStartCompleted ();
	void waitForStopCompleted ();
private:
	std::vector<int> m_fds;
	std::mutex m_fdsMutex;
	std::mutex m_notifyMutex;

	int m_pipeRead;
	int m_pipeWrite;

	std::condition_variable m_startCompleted;
	std::condition_variable m_stopFromPipeCond;
	bool m_isWaiting	= false;
	bool m_stopFromPipe = false;
};

void
EasySelect::Impl::addFd (int fd)
{
	std::lock_guard<std::mutex> lock (m_fdsMutex);
	m_fds.emplace_back (fd);
}

void
EasySelect::Impl::removeFd (int fd)
{
	std::lock_guard<std::mutex> lock (m_fdsMutex);
	auto fdIt = std::find (m_fds.begin(), m_fds.end(), fd);
	if (fdIt == m_fds.end())
		return;
	m_fds.erase (fdIt);
}

void
EasySelect::Impl::stop()
{
	char ANY_CHAR = 0;
	IOUtil::writen (m_pipeWrite, &ANY_CHAR, 1);
}

void
EasySelect::Impl::waitForStartCompleted()
{
	std::mutex mutex;
	std::unique_lock<std::mutex> lock (mutex);
	m_startCompleted.wait (lock, [&] { return m_isWaiting == false; });
}

void
EasySelect::Impl::waitForStopCompleted()
{
	std::unique_lock<std::mutex> lock (m_notifyMutex);
	m_stopFromPipeCond.wait (lock, [&] { return m_stopFromPipe == true; });
	m_stopFromPipe = false;
}

std::vector<int>
EasySelect::Impl::waitForReadAble()
{
	fd_set fdSet;
	FD_ZERO (&fdSet);
	int fd_limits = -1;
	std::for_each (m_fds.begin(), m_fds.end(), [&] (int fd) {
		FD_SET (fd, &fdSet);
		fd_limits = std::max (fd_limits, fd);
	});
	FD_SET (m_pipeRead, &fdSet); //把管道读端放进去
	fd_limits = std::max (fd_limits, m_pipeRead);
	++fd_limits;
	{
		m_isWaiting = true;
		m_startCompleted.notify_one();
	}
	int nReadAble = ::select (fd_limits, &fdSet, NULL, NULL, NULL);
	if (nReadAble == -1) {
		setError (strerror (errno));
		return {};
	}
	m_isWaiting = false;
	std::vector<int> ret;
	std::for_each (m_fds.begin(), m_fds.end(), [&] (int fd) {
		if (FD_ISSET (fd, &fdSet)) {
			ret.emplace_back (fd);
		}
	});

	if (FD_ISSET (m_pipeRead, &fdSet)) {
		std::lock_guard<std::mutex> lock (m_notifyMutex);
		m_stopFromPipe = true;
		m_stopFromPipeCond.notify_one();
		char ANY_CHAR;
		IOUtil::readn (m_pipeRead, &ANY_CHAR, 1);
	}

	return ret;
}

///////////////////////////////////////////////////////////

EasySelect::EasySelect() : m_pImpl (std::make_shared<Impl>())
{
	assert (m_pImpl);
}

std::vector<int>
EasySelect::waitForReadAble()
{
	assert (m_pImpl);
	return m_pImpl->waitForReadAble();
}

void
EasySelect::waitForStartCompleted()
{
	assert (m_pImpl);
	return m_pImpl->waitForStartCompleted();
}

void
EasySelect::waitForStopCompleted()
{
	assert (m_pImpl);
	return m_pImpl->waitForStopCompleted();
}

void
EasySelect::stop()
{
	assert (m_pImpl);
	return m_pImpl->stop();
}

void
EasySelect::addFd (int fd)
{
	assert (m_pImpl);
	return m_pImpl->addFd (fd);
}

void
EasySelect::removeFd (int fd)
{
	assert (m_pImpl);
	return m_pImpl->removeFd (fd);
}
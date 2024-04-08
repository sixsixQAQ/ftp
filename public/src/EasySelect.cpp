#include "EasySelect.hpp"

#include "BaseUtil.hpp"

#include <algorithm>
#include <cassert>
#include <mutex>
#include <stdexcept>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

class EasySelect::Impl {
public:

	Impl (bool startNow);
	~Impl();
	void start ();
	void addFd (int fd, Callback callback, bool withLock = true);
	void removeFd (int fd, bool withLock = true);
	void stop ();

private:
	std::vector<int> waitForReadable ();
	std::vector<int> m_fds;

	std::unordered_map<int, Callback> m_callbackMap;
	std::thread m_workThread;
	std::mutex m_mutex;

	bool m_isRunning = false;
	std::pair<int, int> m_pipe;
	void openPipe (bool withLock);
	void closePipe (bool wihLock);
	bool m_shouldThreadReturn = false;
};

void
EasySelect::Impl::addFd (int fd, Callback callback, bool withLock)
{
	std::unique_lock<std::mutex> lock (m_mutex, std::defer_lock);
	if (withLock)
		lock.lock();
	m_fds.emplace_back (fd);
	m_callbackMap.insert ({fd, callback});
}

void
EasySelect::Impl::removeFd (int fd, bool withLock)
{
	std::unique_lock<std::mutex> lock (m_mutex, std::defer_lock);
	if (withLock)
		lock.lock();
	m_fds.erase (std::find (m_fds.begin(), m_fds.end(), fd));
	m_callbackMap.erase (fd);
}

EasySelect::Impl::Impl (bool startNow)
{
	if (startNow) {
		start();
	}
}

EasySelect::Impl::~Impl()
{
	stop();
}

void
EasySelect::Impl::start()
{
	std::lock_guard<std::mutex> lock (m_mutex);
	if (m_isRunning)
		return;
	openPipe (false);
	m_workThread = std::thread (
		[] (Impl *selector, const std::unordered_map<int, Callback> &map, bool &shouldReturn) {
			while (true) {

				std::vector<int> fds = selector->waitForReadable();

				for (auto fd : fds) {
					auto callbackIt = map.find (fd);
					assert (callbackIt != map.end() && "确保map和vector中fd同步");
					if (!callbackIt->second)
						continue;

					callbackIt->second (fd);
					if (shouldReturn) {
						return;
					}
				}
			}
		},
		this,
		std::ref (m_callbackMap),
		std::ref (m_shouldThreadReturn)
	);
	m_isRunning = true;
}

void
EasySelect::Impl::stop()
{
	std::lock_guard<std::mutex> lock (m_mutex);
	if (m_isRunning == false)
		return;

	if (m_workThread.joinable()) {
		char ANY_CHAR = 0;
		IOUtil::writen (m_pipe.second, &ANY_CHAR, sizeof (ANY_CHAR));
		m_workThread.join();
		closePipe (false);
	}
	m_isRunning = false;
}

std::vector<int>
EasySelect::Impl::waitForReadable()
{
	fd_set fdSet;
	FD_ZERO (&fdSet);

	assert (m_fds.size() != 0 && "至少应有管道fd");
	assert (m_fds.size() == m_callbackMap.size() && "确保map和vector中fd同步");

	int fd_limits = -1;
	std::for_each (m_fds.begin(), m_fds.end(), [&] (int fd) {
		FD_SET (fd, &fdSet);
		fd_limits = std::max (fd_limits, fd);
	});

	++fd_limits;

	int nReadAble = ::select (fd_limits, &fdSet, NULL, NULL, NULL);
	if (nReadAble == -1) {
		setError (strerror (errno));
		return {};
	}

	std::vector<int> ret;
	std::for_each (m_fds.begin(), m_fds.end(), [&] (int fd) {
		if (FD_ISSET (fd, &fdSet)) {
			ret.emplace_back (fd);
		}
	});

	return ret;
}

void
EasySelect::Impl::openPipe (bool withLock)
{
	m_shouldThreadReturn = false;
	int pipeFds[2];
	if (pipe (pipeFds) == -1) {
		setError (strerror (errno));
		return;
	}
	m_pipe = std::pair<int, int> (pipeFds[0], pipeFds[1]);


	addFd (
		m_pipe.first,
		[&] (int fd) {
			char ANY_CHAR;
			IOUtil::readn (fd, &ANY_CHAR, 1);
			m_shouldThreadReturn = true;
		},
		withLock
	);
}

void
EasySelect::Impl::closePipe (bool withLock)
{
	removeFd (m_pipe.first, withLock);
	::close (m_pipe.first);
	::close (m_pipe.second);
}

///////////////////////////////////////////////////////////

EasySelect::EasySelect (bool start) : m_pImpl (std::make_shared<Impl> (start))
{
	assert (m_pImpl);
}

void
EasySelect::start()
{
	assert (m_pImpl);
	return m_pImpl->start();
}

void
EasySelect::addFd (int fd, Callback callback)
{
	assert (m_pImpl);
	return m_pImpl->addFd (fd, callback);
}

void
EasySelect::removeFd (int fd)
{
	assert (m_pImpl);
	return m_pImpl->removeFd (fd);
}

void
EasySelect::stop()
{
	assert (m_pImpl);
	return m_pImpl->stop();
}

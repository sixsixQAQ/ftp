#include "EasySelect.hpp"

#include "BaseUtil.hpp"

#include <cassert>
#include <mutex>
#include <stdexcept>
#include <string.h>
#include <thread>
#include <unistd.h>

class EasySelect::Impl {
public:

	Impl (bool startNow);
	~Impl();
	void start ();
	void addFd (int fd, Callback callback);
	void removeFd (int fd);
	void stop ();

private:
	std::vector<int> waitForReadable ();
	std::vector<int> m_fds;

	std::unordered_map<int, Callback> m_callbackMap;
	std::thread m_workThread;
	std::mutex m_mutex;

	bool m_isRunning = false;
	std::pair<int, int> m_pipe;
	void openPipe ();
	void closePipe ();
};

void
EasySelect::Impl::addFd (int fd, Callback callback)
{
	std::lock_guard<std::mutex> lock (m_mutex);
	m_fds.emplace_back (fd);
	m_callbackMap.insert ({fd, callback});
}

void
EasySelect::Impl::removeFd (int fd)
{
	std::lock_guard<std::mutex> lock (m_mutex);
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
	if (m_isRunning)
		return;
	openPipe();

	m_workThread = std::thread ([&] {
		while (true) {

			std::vector<int> fds = waitForReadable();

			for (auto fd : fds) {
				auto callbackIt = m_callbackMap.find (fd);
				assert (callbackIt != m_callbackMap.end() && "确保map和vector中fd同步");
				if (!callbackIt->second)
					continue;
				try {
					callbackIt->second (fd);
				} catch (...) {
					return;
				}
			}
		}
	});
	m_isRunning	 = true;
}

void
EasySelect::Impl::stop()
{
	if (m_isRunning == false)
		return;

	if (m_workThread.joinable()) {
		char ANY_CHAR = 0;
		IOUtil::writen (m_pipe.second, &ANY_CHAR, sizeof (ANY_CHAR));
		m_workThread.join();
		closePipe();
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
EasySelect::Impl::openPipe()
{
	int pipeFds[2];
	if (pipe (pipeFds) == -1) {
		setError (strerror (errno));
		return;
	}
	m_pipe = std::pair<int,int> (pipeFds[0], pipeFds[1]);

	addFd (m_pipe.first, [&] (int fd) {
		char ANY_CHAR;
		IOUtil::readn (fd, &ANY_CHAR, 1);
		throw std::runtime_error ("pipe readable");
	});
}

void
EasySelect::Impl::closePipe()
{
	removeFd (m_pipe.first);
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

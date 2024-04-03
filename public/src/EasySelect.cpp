#include "EasySelect.hpp"
#include "BaseUtil.hpp"
#include <cassert>
#include <string.h>
#include <unistd.h>

class EasySelect::Impl
{
  public:
    using Callback = std::function<void(int fd)>;

    Impl();

    void startSelect();
    void addFd(const std::pair<int, Callback> &);
    void removeFd(int fd);
    void stopSelect();

  private:
    std::vector<int> selectOnce();
    std::vector<int> m_fds;
    std::pair<int, int> m_pipe;

    std::unordered_map<int, Callback> m_callbackMap;
    bool m_pipeReadAble = false;
};

void EasySelect::Impl::addFd(const std::pair<int, Callback> &pair)
{
    stopSelect();
    m_fds.emplace_back(pair.first);
    m_callbackMap.insert(pair);
    startSelect();
}

void EasySelect::Impl::removeFd(int fd)
{
    stopSelect();
    m_fds.erase(std::find(m_fds.begin(), m_fds.end(), fd));
    m_callbackMap.erase(fd);
    startSelect();
}

EasySelect::Impl::Impl()
{
    int pipeFds[2];
    if (pipe(pipeFds) == -1)
    {
        setError(strerror(errno));
        return;
    }
    m_pipe = std::pair(pipeFds[0], pipeFds[1]);

    addFd({m_pipe.first, [&](int fd) { m_pipeReadAble = true; }});
}

void EasySelect::Impl::startSelect()
{
    m_pipeReadAble = false;
    while (true)
    {
        std::vector<int> fds = selectOnce();

        for (auto fd : fds)
        {
            auto callbackIt = m_callbackMap.find(fd);
            assert(callbackIt != m_callbackMap.end());
            if (!callbackIt->second)
                continue;
            callbackIt->second(fd);
        }
        if (m_pipeReadAble)
        {
            return;
        }
    }
}

void EasySelect::Impl::stopSelect()
{
    char ANY_CHAR;
    IOUtil::writen(m_pipe.second, &ANY_CHAR, sizeof(ANY_CHAR));
}

std::vector<int> EasySelect::Impl::selectOnce()
{
    fd_set fdSet;
    FD_ZERO(&fdSet);

    assert(m_fds.size() != 0);
    assert(m_fds.size() == m_callbackMap.size());

    int fd_limits = -1;
    std::for_each(m_fds.begin(), m_fds.end(), [&](int fd) {
        FD_SET(fd, &fdSet);
        fd_limits = std::max(fd_limits, fd);
    });

    ++fd_limits;

    int nReadAble = ::select(fd_limits, &fdSet, NULL, NULL, NULL);
    if (nReadAble == -1)
    {
        setError(strerror(errno));
        return {};
    }

    std::vector<int> ret;
    std::for_each(m_fds.begin(), m_fds.end(), [&](int fd) {
        if (FD_ISSET(fd, &fdSet))
        {
            ret.emplace_back(fd);
        }
    });

    return ret;
}

///////////////////////////////////////////////////////////

EasySelect::EasySelect() : m_pImpl(std::make_shared<Impl>())
{
    assert(m_pImpl);
}

void EasySelect::startSelect()
{
    assert(m_pImpl);
    return m_pImpl->startSelect();
}

void EasySelect::addFd(const std::pair<int, Callback> &pair)
{
    assert(m_pImpl);
    return m_pImpl->addFd(pair);
}

void EasySelect::removeFd(int fd)
{
    assert(m_pImpl);
    return m_pImpl->removeFd(fd);
}

void EasySelect::stopSelect()
{
    assert(m_pImpl);
    return m_pImpl->stopSelect();
}

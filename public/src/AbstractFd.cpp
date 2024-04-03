#include "AbstractFd.hpp"

#include <unistd.h>

class ControlFd::Impl
{
  public:
    Impl(int fd) : m_fd(fd)
    {
    }
    int getFd() const
    {
        return m_fd;
    }
    void close()
    {
        ::close(m_fd);
        m_fd = -1;
    }

  private:
    int m_fd;
};

ControlFd::ControlFd(int fd) : m_pImpl(std::make_shared<Impl>(fd))
{
}

int ControlFd::getFd() const
{
    if (m_pImpl)
        return m_pImpl->getFd();
    return -1;
}

void ControlFd::close()
{
    if (m_pImpl)
        return m_pImpl->close();
}

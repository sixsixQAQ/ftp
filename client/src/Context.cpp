#include "Context.hpp"
#include <unistd.h>

class Toggle::Impl
{
  public:
    void toggle()
    {
        m_toggle = !m_toggle;
    }
    void turnOn()
    {
        m_toggle = true;
    }
    void turnOff()
    {
        m_toggle = false;
    }
    operator bool()
    {
        return m_toggle;
    }

  private:
    bool m_toggle = false;
};

Toggle::Toggle() : m_pImpl(std::make_shared<Impl>())
{
}

void Toggle::toggle()
{
    if (m_pImpl)
        return m_pImpl->toggle();
}

void Toggle::turnOn()
{
    if (m_pImpl)
        return m_pImpl->turnOn();
}

void Toggle::turnOff()
{
    if (m_pImpl)
        return m_pImpl->turnOff();
}

Toggle::operator bool() const
{
    if (m_pImpl)
        return m_pImpl->operator bool();
    return false;
}
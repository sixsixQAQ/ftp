#pragma once

#include <memory>

class Toggle
{
  public:
    Toggle();
    void toggle();
    void turnOn();
    void turnOff();
    operator bool() const;

  private:
    class Impl;
    std::shared_ptr<Impl> m_pImpl = nullptr;
};

class ControlFd
{
  public:
    ControlFd(int fd);
    int getFd() const;
    operator int() const
    {
        return getFd();
    }

  private:
    class Impl;
    std::shared_ptr<Impl> m_pImpl = nullptr;
};

class DataFd : private ControlFd
{
  public:
    using ControlFd::ControlFd;
    using ControlFd::getFd;
    using ControlFd::operator int;
};

struct Context
{
    ControlFd ctrlFd;
    DataFd dataFd;
    Toggle PASV_Toggle;
    Toggle PORT_Toggle;
};
#pragma once

#include <iostream>
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
    void close();
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
    using ControlFd::close;
    using ControlFd::ControlFd;
    using ControlFd::getFd;
    using ControlFd::operator int;
};

struct Context
{
    Context(std::istream &inStream, std::ostream &outStream)
        : inStream(inStream), outStream(outStream), ctrlFd(-1), dataFd(-1)
    {
      PASV_Toggle.turnOn();
    }
    std::istream &inStream;
    std::ostream &outStream;

    ControlFd ctrlFd;
    DataFd dataFd;
    Toggle PASV_Toggle;
    Toggle PORT_Toggle;
};
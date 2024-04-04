#pragma once

#include <memory>

class ControlFd {
public:
	ControlFd (int fd);
	int getFd () const;
	void close ();

	operator int () const
	{
		return getFd();
	}

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

class DataFd : private ControlFd {
public:
	using ControlFd::close;
	using ControlFd::ControlFd;
	using ControlFd::getFd;
	using ControlFd::operator int;
};
#pragma once

#include "ErrorUtil.hpp"

#include <functional>
#include <memory>
#include <vector>

class EasySelect : public ErrorUtil {
public:
	using Callback = std::function<void (int fd)>;

	EasySelect (bool start = true);
	void restart ();
	void addFd (int fd, Callback callback);
	void removeFd (int fd);
	void stop ();

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
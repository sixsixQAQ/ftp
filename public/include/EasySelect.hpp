#pragma once

#include "ErrorUtil.hpp"

#include <functional>
#include <memory>
#include <vector>

class EasySelect : public ErrorUtil {
public:
	using Callback = std::function<void (int fd)>;

	EasySelect();
	void start ();
	void addFd (const std::pair<int, Callback> &);
	void removeFd (int fd);
	void stop ();

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
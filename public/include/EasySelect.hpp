#pragma once

#include "ErrorUtil.hpp"

#include <functional>
#include <memory>
#include <vector>

class EasySelect : public ErrorUtil {
public:
	EasySelect();
	std::vector<int> waitForReadAble ();
	void stop ();

	void waitForStartCompleted ();
	void waitForStopCompleted ();

	void addFd (int fd);
	void removeFd (int fd);
private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};
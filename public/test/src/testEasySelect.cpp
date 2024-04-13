#include "testEasySelect.hpp"

#include "EasySelect.hpp"

#include <assert.h>
#include <iostream>
#include <thread>
#include <unistd.h>

void
testEasySelect ()
{
	EasySelect selector;
	std::thread waitThread ([&] () {
		selector.addFd (STDIN_FILENO);
		auto readAbleFds = selector.waitForReadAble();
		assert (readAbleFds.size() == 0);
		std::cout << "EasySelect Test OK.\n";
	});
	selector.waitForStartCompleted();
	selector.stop();
	waitThread.join();
}
#pragma once

#include "AbstractFd.hpp"
#include "Toggle.hpp"

#include <iostream>
#include <memory>

struct Context {
	Context (std::istream &inStream, std::ostream &outStream) :
		inStream (inStream), outStream (outStream), ctrlFd (-1), dataFd (-1)
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
#pragma once

#include "AbstractFd.hpp"

#include <iostream>
#include <memory>

class Toggle {
public:
	Toggle();
	void toggle ();
	void turnOn ();
	void turnOff ();
	operator bool () const;

private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

struct Context {
	Context (std::istream &inStream, std::ostream &outStream) :
		inStream (inStream), outStream (outStream), ctrlFd (-1), dataFd (-1)
	{
		PASV_Toggle.turnOn();
	}

	std::istream &inStream;
	std::ostream &outStream;

	AbstractFd ctrlFd;
	DataFd dataFd;
	Toggle PASV_Toggle;
	Toggle PORT_Toggle;
};
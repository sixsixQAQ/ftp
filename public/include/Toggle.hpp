#pragma once

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
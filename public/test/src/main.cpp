#include "BackableReader.hpp"
#include "testBackableReader.hpp"
#include "testEasySelect.hpp"

#include <iostream>

int
main (void)
{
	std::cout << "test Start\n";

	// testBackableReader();
	testEasySelect();
	std::cout << "All tests passed.\n";
}
#include "BackableReader.hpp"
#include "macro.hpp"

#include <assert.h>
#include <unistd.h>

void
testBackableReader ()
{
	int pipeFds[2];
	SUCCEED (pipe (pipeFds) != -1);


	int readFd	= pipeFds[0];
	int writeFd = pipeFds[1];

	BackableReader reader (readFd);
	assert (reader.getCached() == "");

	std::string str = "hello world\r\n";
	SUCCEED (write (writeFd, str.c_str(), str.size()) == (int)str.size());

	char buf[1024];
	SUCCEED (reader.read (buf, sizeof (buf)) == (int)str.size());

	assert (reader.getCached() == "");
	reader.unread (buf, str.size() - 2);
	assert (reader.getCached() == "hello world");

	close (readFd);
	close (writeFd);
}
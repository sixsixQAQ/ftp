#include <stdexcept>
#include <string>

#define SUCCEED(expr)                                                                                        \
	do {                                                                                                     \
		if ((expr) == false)                                                                                 \
			throw std::runtime_error (std::string (                                                          \
										  "FAILED:" + std::string (__FILE__) + "," +                         \
										  std::string (__FUNCTION__) + std::to_string (__LINE__)             \
			)                                                                                                \
										  .c_str());                                                         \
	} while (0);

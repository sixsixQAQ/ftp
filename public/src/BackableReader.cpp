#include "BackableReader.hpp"

int BackableReader::read(void *buf, size_t size) { return 0; }

void BackableReader::rollBackData(void *buf, size_t size) {}

std::string BackableReader::getCached() { return std::string(); }
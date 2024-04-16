#pragma once

#include "AbstractFd.hpp"
#include "ProtocolCmdSender.hpp"

#include <iostream>

struct Context {

	enum class TransferMode { PASV, PORT };

	Context (std::istream &inStream, std::ostream &outStream) : inStream (inStream), outStream (outStream)
	{
	}

	std::istream &inStream;
	std::ostream &outStream;
	ControlFd ctrlFd										 = -1;
	DataFd dataFd											 = -1;
	std::string passiveIp									 = "";
	uint16_t passivePort									 = 0;
	TransferMode transferMode								 = TransferMode::PASV;
	ProtocolCmdSender::RepresentationType representationType = ProtocolCmdSender::RepresentationType::BINARY;
};
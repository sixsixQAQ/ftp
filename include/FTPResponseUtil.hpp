#pragma once

#include "Context.hpp"
#include "ErrorUtil.hpp"
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class FTPResponseUtil : public ErrorUtil
{

    // "214-The following commands are recognized.\r\n"
    // " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"
    // " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"
    // " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"
    // "214 Help OK.\r\n"
  public:
    static std::vector<std::string> getResponse(ControlFd connFd);
    static std::vector<std::string> echoResponse(ControlFd connFd, std::ostream &out = std::cout);
    static void PASVResponse(ControlFd connFd, std::string &ip, uint16_t &port);
    static long sizeResponse(ControlFd connFd);
};
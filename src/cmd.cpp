#include <string>
#include <unordered_map>
#include <vector>

#include "BaseUtil.hpp"
#include "FTPResponseUtil.hpp"
#include "cmd.hpp"
#include <iostream>

struct Command
{
  public:
  private:
    std::string m_name;
};

ControlFd openImpl(const std::string &domain, uint16_t port)
{
    int connFd = NetUtil::connectToServer(domain, port);
    if (connFd == -1)
    {
        return -1;
    }

    FTPResponseUtil::echoResponse(connFd);

    return connFd;
}

void passiveImpl(Toggle &mode)
{
    mode.toggle();
}

void pwdImpl(const ControlFd &fd)
{
    FTPUtil::sendCmd(fd, {"PWD"});
    FTPResponseUtil::echoResponse(fd);
}

void deleteImpl(const ControlFd &fd, const std::string &filePath)
{
    FTPUtil::sendCmd(fd, {"DELE", filePath});
    FTPResponseUtil::echoResponse(fd);
}

void rmdirImpl(const ControlFd &fd, const std::string &dirPath)
{
    FTPUtil::sendCmd(fd, {"RMD", dirPath});
    FTPResponseUtil::echoResponse(fd);
}

void mkdirImpl(const ControlFd &fd, const std::string &dirPath)
{
    FTPUtil::sendCmd(fd, {"MKD", dirPath});
    FTPResponseUtil::echoResponse(fd);
}

void lsImpl(const ControlFd &fd, const Toggle &isPassive, const std::string &path)
{
    if (isPassive)
    {
        FTPUtil::sendCmd(fd, {"PASV"});
        
        std::string ip;
        uint16_t port;
        FTPResponseUtil::PASVResponse(fd, ip, port);

        FTPUtil::sendCmd(fd, {"LIST", path});

    }
}

void quitImpl(int connFd)
{
    FTPUtil::sendCmd(connFd, {"QUIT"});
    FTPResponseUtil::echoResponse(connFd);
    exit(EXIT_SUCCESS);
}

// std::unordered_map<std:string>

// {"open", "open", "open <ip> [port]", open_handler},
// {"help", "help", "help [command]", help_handler},
// {"USER", "USER", "USER <SP> <username> <CRLF>", USER_handler},
// {"PASS", "PASS", "PASS <SP> <password> <CRLF>", PASS_handler},
// {"ACCT", "ACCT", "ACCT <SP> <account-information> <CRLF>", ACCT_handler},
// {"CWD", "CWD", "CWD <SP> <pathname> <CRLF>", CWD_handler},
// {"CDUP", "CDUP", "CDUP <CRLF>", CDUP_handler},
// {"SMNT", "SMNT", "SMNT <SP> <pathname> <CRLF>", SMNT_handler},
// {"QUIT", "QUIT", "QUIT <CRLF>", QUIT_handler},
// {"REIN", "REIN", "REIN <CRLF>", REIN_handler},
// {"PORT", "PORT", "PORT <SP> <host-port> <CRLF>", PORT_handler},
// {"PASV", "PASV", "PASV <CRLF>", PASV_handler},
// {"TYPE", "TYPE", "TYPE <SP> <type-code> <CRLF>", TYPE_handler},
// {"STRU", "STRU", "STRU <SP> <structure-code> <CRLF>", STRU_handler},
// {"MODE", "MODE", "MODE <SP> <mode-code> <CRLF>", MODE_handler},
// {"RETR", "RETR", "RETR <SP> <pathname> <CRLF>", RETR_handler},
// {"STOR", "STOR", "STOR <SP> <pathname> <CRLF>", STOR_handler},
// {"STOU", "STOU", "STOU <CRLF>", STOU_handler},
// {"APPE", "APPE", "APPE <SP> <pathname> <CRLF>", APPE_handler},
// {"ALLO", "ALLO", "ALLO <SP> <decimal-integer> [<SP> R <SP> <decimal-integer>] <CRLF>", ALLO_handler},
// {"REST", "REST", "REST <SP> <marker> <CRLF>", REST_handler},
// {"RNFR", "RNFR", "RNFR <SP> <pathname> <CRLF>", RNFR_handler},
// {"RNTO", "RNTO", "RNTO <SP> <pathname> <CRLF>", RNTO_handler},
// {"ABOR", "ABOR", "ABOR <CRLF>", ABOR_handler},
// {"DELE", "DELE", "DELE <SP> <pathname> <CRLF>", DELE_handler},
// {"RMD", "RMD", "RMD <SP> <pathname> <CRLF>", RMD_handler},
// {"MKD", "MKD", "MKD <SP> <pathname> <CRLF>", MKD_handler},
// {"PWD", "PWD", "PWD <CRLF>", PWD_handler},
// {"LIST", "LIST", "LIST [<SP> <pathname>] <CRLF>", LIST_handler},
// {"NLST", "NLST", "NLST [<SP> <pathname>] <CRLF>", NLST_handler},
// {"SITE", "SITE", "SITE <SP> <string> <CRLF>", SITE_handler},
// {"SYST", "SYST", "SYST <CRLF>", SYST_handler},
// {"STAT", "STAT", "STAT [<SP> <pathname>] <CRLF>", STAT_handler},
// {"HELP", "HELP", "HELP [<SP> <string>] <CRLF>", HELP_handler},
// {"NOOP", "NOOP", "NOOP <CRLF>", NOOP_handler},
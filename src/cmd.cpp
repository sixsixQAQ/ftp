#include <string>
#include <unordered_map>
#include <vector>

#include "BaseUtil.hpp"
#include "FTPResponseUtil.hpp"
#include "cmd.hpp"
#include <iostream>
#include <sstream>
#include <thread>

struct Command
{
  public:
  private:
    std::string m_name;
};

ControlFd openImpl(const std::string &domain, uint16_t port)
{
    ControlFd connFd = NetUtil::connectToServer(domain, port);
    if (connFd == -1)
    {
        return -1;
    }
    FTPResponseUtil::echoResponse(connFd);

    return connFd;
}

void _userImpl_(const ControlFd &fd, const std::string &userName)
{
    FTPUtil::sendCmd(fd, {"USER", userName});
    FTPResponseUtil::echoResponse(fd);
}

void _passwordImpl_(const ControlFd &fd, const std::string &passwd)
{
    FTPUtil::sendCmd(fd, {"PASS", passwd});
    FTPResponseUtil::echoResponse(fd);
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

static void withPassiveMode(const ControlFd &fd, std::function<void(DataFd dataFd)> callback)
{
    FTPUtil::sendCmd(fd, {"PASV"});
    std::string ip;
    uint16_t port;
    FTPResponseUtil::PASVResponse(fd, ip, port);

    DataFd dataFd = NetUtil::connectToServer(ip, port);

    callback(dataFd);

    dataFd.close();
}

std::string lsImpl(const ControlFd &fd, const Toggle &isPassive, const std::string &path)
{
    if (isPassive)
    {
        std::stringstream retStream;

        withPassiveMode(fd, [&](DataFd dataFd) {
            FTPUtil::sendCmd(fd, {"LIST", path});

            FTPResponseUtil::echoResponse(fd, retStream);
            retStream << NetUtil::readAll(dataFd);
        });

        return retStream.str();
    }
    return "";
}

void quitImpl(int connFd)
{
    FTPUtil::sendCmd(connFd, {"QUIT"});
    FTPResponseUtil::echoResponse(connFd);
    exit(EXIT_SUCCESS);
}

void renameImpl(const ControlFd &fd, const std::string &oldname, const std::string &newname)
{
    FTPUtil::sendCmd(fd, {"RNFR", oldname});
    FTPResponseUtil::getResponse(fd);
    FTPUtil::sendCmd(fd, {"RNTO", newname});
    FTPResponseUtil::echoResponse(fd);
}

void putImpl(const ControlFd &fd, const std::string &localPath, const std::string &remotePath)
{
    withPassiveMode(fd, [&](DataFd dataFd) {
        FTPUtil::sendCmd(fd, {"STOR", remotePath});
        FTPResponseUtil::echoResponse(fd);

        NetUtil::syncLocalToRemote(dataFd, localPath);
    });
    FTPResponseUtil::echoResponse(fd);
}

void getImpl(const ControlFd &fd, const std::string &remotePath, const std::string &localPath)
{
    withPassiveMode(fd, [&](DataFd dataFd) {
        FTPUtil::sendCmd(fd, {"RETR", remotePath});
        FTPResponseUtil::echoResponse(fd);

        NetUtil::syncRemoteToLocal(dataFd, localPath);
    });
    FTPResponseUtil::echoResponse(fd);
}
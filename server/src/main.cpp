#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "BaseUtil.hpp"
#include <thread>
#include <vector>

#include "AbstractFd.hpp"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include "EasySelect.hpp"

class SyncReader
{
  public:
    using CallbackFunc = std::function<void(int fd)>;
    SyncReader() = default;

    void start();
    void stop();
    void addTask(int fd, CallbackFunc callback = nullptr);
    void setCallback(int fd, CallbackFunc callback);
    void removeTask(int fd);

  private:
    std::unordered_map<int, CallbackFunc> m_taskMap;
    std::thread m_monitorThread;
    std::condition_variable m_newClientCome;
    std::mutex m_mutex;
};

void SyncReader::start()
{
    stop();
    m_monitorThread = std::thread([&] {
        
    });
    m_monitorThread.detach();
}

void SyncReader::stop()
{
}

void SyncReader::addTask(int fd, CallbackFunc callback)
{
    m_taskMap.insert({fd, callback});
    m_newClientCome.notify_one();
}

void SyncReader::setCallback(int fd, CallbackFunc callback)
{
}

void SyncReader::removeTask(int fd)
{
    m_taskMap.erase(fd);
}

std::vector<std::thread> threadPool;

void serveForClient(int connfd, struct sockaddr_in clienAddr)
{
    std::shared_ptr<SyncReader> reader = nullptr;
    if (reader == nullptr)
    {
        reader = std::make_shared<SyncReader>();
        reader->start();
    }

    std::string acceptMessage = "220 FreeFTP-Server v1.0\r\n";
    IOUtil::writen(connfd, acceptMessage.c_str(), acceptMessage.length());

    reader->addTask(connfd, [&](int fd) {
        char buf[4096];
        int nRead = read(fd, buf, sizeof(buf));
        if (nRead < 0)
        {
            perror("read()");
            return;
        }
        else if (nRead == 0)
        {
            close(fd);
            reader->removeTask(fd);
            return;
        }
        else
        {

            std::cout.write(buf, nRead) << "\n";
        }
    });
}

void start()
{
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(21);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1)
    {
        perror("socket()");
        return;
    }

    if (bind(listenFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        close(listenFd);
        perror("bind()");
        return;
    }
    constexpr int LISTEN_QUEUE_SIZE = 10;
    if (listen(listenFd, LISTEN_QUEUE_SIZE) == -1)
    {
        close(listenFd);
        perror("listen()");
        return;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    while (1)
    {
        int connfd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (connfd == -1)
        {
            perror("accept()");
            continue;
        }
        else
        {
            serveForClient(connfd, clientAddr);
        }
    }
}

int main(void)
{
    std::cout << "FreeFTP-Server v1.0\n";
    start();
}

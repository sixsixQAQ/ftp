#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include "raw_handler.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void syncFile(int inFd, int outFd)
{
    char buf[4096];
    for (;;)
    {
        int nRead = read(inFd, buf, sizeof(buf));
        if (nRead < 0)
        {
            perror("read()");
            return;
        }
        else if (nRead == 0)
        {
            return;
        }

        if (outFd == -1)
        {
            continue;
        }

        int totalWrite = 0;
        for (; totalWrite < nRead;)
        {
            int nWrite = write(outFd, buf + totalWrite, nRead - totalWrite);
            if (nWrite < 0)
            {
                perror("write()");
                return;
            }
            totalWrite += nWrite;
        }
    }
}

void RETR_handler(int argc, const char **argv)
{
    if (argc != 2)
    {
        return;
    }

    PASV_handler(0, NULL);
    uint16_t port = get_shared_context()->PASV_port;
    const char *ip = get_shared_context()->PASV_ip;

    struct sockaddr_in servAddr;
    servAddr.sin_port = htons(port);
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servAddr.sin_addr);

    int dataConnFd = socket(AF_INET, SOCK_STREAM, 0);
    if (dataConnFd == -1)
    {
        perror("socket()");
        return;
    }
    if (connect(dataConnFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        close(dataConnFd);
        perror("connect()");
        return;
    }

    int connFd = get_shared_context()->control_connection_fd;
    char RETR_CMD[CMD_MAX_SIZE];
    snprintf(RETR_CMD, sizeof(RETR_CMD), "RETR %s\r\n", argv[1]);
    write(connFd, RETR_CMD, strlen(RETR_CMD));
    echo_response(connFd);

    int outFd = open(argv[1], O_WRONLY | O_CREAT, 0644);
    if (outFd == -1)
    {
        perror("open()");
    }

    syncFile(dataConnFd, outFd);

    close(dataConnFd);
    close(outFd);

    echo_response(connFd);
}
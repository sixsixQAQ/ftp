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

static void writeWholeFile(int sockfd, const char *filePath)
{
    int fd = open(filePath, O_RDONLY);
    if (fd == -1)
    {
        perror("open()");
        return;
    }

    char buf[4096];
    for (;;)
    {
        int nRead = read(fd, buf, sizeof(buf));
        if (nRead < 0)
        {
            perror("read()");
            goto DESTRUCT;
        }
        else if (nRead == 0)
        {
            goto DESTRUCT;
        }

        int totalWrite = 0;
        for (; totalWrite < nRead;)
        {
            int nWrite = write(sockfd, buf + totalWrite, nRead - totalWrite);
            if (nWrite < 0)
            {
                perror("write()");
                goto DESTRUCT;
            }
            totalWrite += nWrite;
        }
    }

DESTRUCT:
    close(fd);
    return;
}
void STOR_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;

    PASV_handler(0, NULL);

    uint16_t port = get_shared_context()->PASV_port;
    const char *ip = get_shared_context()->PASV_ip;
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
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

    char STOR_CMD[CMD_MAX_SIZE];
    snprintf(STOR_CMD, sizeof(STOR_CMD), "STOR %s\r\n", argv[1]);

    write(connFd, STOR_CMD, strlen(STOR_CMD));
    echo_response(connFd);

    writeWholeFile(dataConnFd, argv[1]);

    close(dataConnFd);

    echo_response(connFd);
}
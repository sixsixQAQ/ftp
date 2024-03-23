#include "context_data.h"
#include "ftplimits.h"
#include "raw_handler.h"
#include <arpa/inet.h>
#include <ftptool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void LIST_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;

    PASV_handler(0, NULL);

    int dataConnFd = socket(AF_INET, SOCK_STREAM, 0);
    if (dataConnFd == -1)
    {
        perror("socket()");
        return;
    }

    // PASV模式接收数据
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(get_shared_context()->PASV_port);
    inet_pton(AF_INET, get_shared_context()->PASV_ip, &servAddr.sin_addr);

    if (connect(dataConnFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        close(dataConnFd);
        perror("connect()");
        return;
    }

    char LIST_CMD[CMD_MAX_SIZE] = "LIST\r\n";
    if (argc == 2)
    {
        snprintf(LIST_CMD, sizeof(LIST_CMD), "LIST %s\r\n", argv[1]);
    }

    int connfd = get_shared_context()->control_connection_fd;
    write(connfd, LIST_CMD, strlen(LIST_CMD));
    echo_response(connfd);

    char buf[4096];
    for (;;)
    {
        int nRead = read(dataConnFd, buf, sizeof(buf));
        if (nRead < 0)
        {
            perror("read()");
            break;
        }
        else if (nRead == 0)
        {
            break;
        }
        write(STDOUT_FILENO, buf, nRead);
    }
    close(dataConnFd);
    echo_response(connfd);
}
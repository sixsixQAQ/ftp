#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"

#include "raw_handler.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void NLST_handler(int argc, const char **argv)
{
    if (argc != 1 && argc != 2)
        return;

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
    char NLST_CMD[CMD_MAX_SIZE];
    snprintf(NLST_CMD, sizeof(NLST_CMD), "NLST %s\r\n", argv[1]);
    write(connFd, NLST_CMD, strlen(NLST_CMD));
    echo_response(connFd);

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
    echo_response(connFd);
}
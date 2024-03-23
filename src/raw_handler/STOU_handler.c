#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include "raw_handler.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void STOU_handler(int argc, const char **argv)
{
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

    int fd = get_shared_context()->control_connection_fd;
    char STOU_CMD[] = "STOU\r\n";
    write(fd, STOU_CMD, strlen(STOU_CMD));
    echo_response(fd);

    /**
     * 这里不做实际文件传输，只是尝试让服务器找一个唯一标识的文件。
     */
    close(dataConnFd);
    echo_response(fd);
}
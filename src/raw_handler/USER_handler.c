#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void USER_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;

    int fd = get_shared_context()->control_connection_fd;
    char USER_CMD[CMD_MAX_SIZE];
    snprintf(USER_CMD, sizeof(USER_CMD), "USER %s\r\n", argv[1]);
    write(fd, USER_CMD, strlen(USER_CMD));
    echo_response(fd);
}
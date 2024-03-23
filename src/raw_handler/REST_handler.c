#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void REST_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char CMD[CMD_MAX_SIZE];
    snprintf(CMD, sizeof(CMD), "REST %s\r\n", argv[1]);
    write(fd, CMD, strlen(CMD));
    echo_response(fd);
}
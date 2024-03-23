#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void RMD_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char RMD_CMD[CMD_MAX_SIZE];
    snprintf(RMD_CMD, sizeof(RMD_CMD), "RMD %s\r\n", argv[1]);
    write(fd, RMD_CMD, strlen(RMD_CMD));
    echo_response(fd);
}
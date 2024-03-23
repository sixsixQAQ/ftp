#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void CWD_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char CWD_CMD[CMD_MAX_SIZE];
    snprintf(CWD_CMD, sizeof(CWD_CMD), "CWD %s\r\n", argv[1]);
    write(fd, CWD_CMD, strlen(CWD_CMD));
    echo_response(fd);
}
#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void PASS_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;

    int fd = get_shared_context()->control_connection_fd;
    char PASS_CMD[CMD_MAX_SIZE];
    snprintf(PASS_CMD, sizeof(PASS_CMD), "PASS %s\r\n", argv[1]);
    write(fd, PASS_CMD, strlen(PASS_CMD));
    echo_response(fd);
}
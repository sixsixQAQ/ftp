#include "context_data.h"
#include "ftptool.h"
#include "ftplimits.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void DELE_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char DELE_CMD[CMD_MAX_SIZE];
    snprintf(DELE_CMD, sizeof(DELE_CMD), "DELE %s\r\n", argv[1]);

    write(fd, DELE_CMD, strlen(DELE_CMD));
    echo_response(fd);
}
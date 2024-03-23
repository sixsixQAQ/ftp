#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void STAT_handler(int argc, const char **argv)
{
    if (argc != 1 && argc != 2)
        return;

    int fd = get_shared_context()->control_connection_fd;
    char STAT_CMD[CMD_MAX_SIZE] = "STAT\r\n";

    if (argc == 2)
    {
        snprintf(STAT_CMD, sizeof(STAT_CMD), "STAT %s\r\n", argv[1]);
    }
    write(fd, STAT_CMD, strlen(STAT_CMD));
    echo_response(fd);
}
#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void RNFR_handler(int argc, const char **argv)
{
    if (argc != 2)
    {
        return;
    }

    int fd = get_shared_context()->control_connection_fd;
    char RNFR_CMD[CMD_MAX_SIZE];
    snprintf(RNFR_CMD, sizeof(RNFR_CMD), "RNFR %s\r\n", argv[1]);
    write(fd, RNFR_CMD, strlen(RNFR_CMD));
    echo_response(fd);
}
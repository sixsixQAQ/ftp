#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void SITE_handler(int argc, const char **argv)
{
    if (argc != 2)
        return;

    int fd = get_shared_context()->control_connection_fd;
    char SITE_CMD[CMD_MAX_SIZE];
    snprintf(SITE_CMD, sizeof(SITE_CMD), "SITE %s\r\n", argv[1]);
    write(fd, SITE_CMD, strlen(SITE_CMD));
    echo_response(fd);
}
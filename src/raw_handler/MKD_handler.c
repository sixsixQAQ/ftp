#include "context_data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ftptool.h"
#include "ftplimits.h"

void MKD_handler(int argc, const char **argv)
{
    if (argc != 2)
    {
        return;
    }
    char buf[CMD_MAX_SIZE];
    snprintf(buf, sizeof(buf), "MKD %s\r\n", argv[1]);

    int fd = get_shared_context()->control_connection_fd;
    write(fd, buf, strlen(buf));

    echo_response(fd);
}
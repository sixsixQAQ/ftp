#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void HELP_handler(int argc, const char **argv)
{
    if (argc != 1 && argc != 2)
        return;

    char HELP_CMD[CMD_MAX_SIZE] = "HELP\r\n";
    if (argc == 2)
    {
        snprintf(HELP_CMD, sizeof(HELP_CMD), "HELP %s\r\n", argv[1]);
    }

    int connfd = get_shared_context()->control_connection_fd;

    write(connfd, HELP_CMD, strlen(HELP_CMD));
    echo_response(connfd);
}

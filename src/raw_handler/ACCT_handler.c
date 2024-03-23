#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void ACCT_handler(int argc, const char **argv)
{
    /**
     * 由于常见的服务器未实现相应功能，这个命令其实也没什么软用。
     */
    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char CMD[CMD_MAX_SIZE];
    snprintf(CMD, sizeof(CMD), "ACCT %s\r\n", argv[1]);
    write(fd, CMD, strlen(CMD));
    echo_response(fd);
}
#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void MODE_handler(int argc, const char **argv)
{
    /**
     * 由于常见的服务器和客户端都支持File结构，这个命令其实也没什么软用。
     * RFC959规定<mode-code> ::= S | B | C
     */

    if (argc != 2)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char MODE_CMD[CMD_MAX_SIZE];
    snprintf(MODE_CMD, sizeof(MODE_CMD), "MODE %s\r\n", argv[1]);
    write(fd, MODE_CMD, strlen(MODE_CMD));
    echo_response(fd);
}
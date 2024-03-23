#include "context_data.h"
#include "ftptool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void ABOR_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;
    char MODE_CMD[] = "ABOR\r\n";
    write(fd, MODE_CMD, strlen(MODE_CMD));
    echo_response(fd);
}
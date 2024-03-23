#include "context_data.h"
#include "ftptool.h"
#include <string.h>
#include <unistd.h>

void NOOP_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;

    char NOOP_CMD[] = "NOOP\r\n";
    write(fd, NOOP_CMD, strlen(NOOP_CMD));
    echo_response(fd);
}
#include "context_data.h"
#include <ftptool.h>
#include <string.h>
#include <unistd.h>

void QUIT_handler(int argc, const char **argv)
{
    if (argc != 1)
        return;
    int fd = get_shared_context()->control_connection_fd;

    char QUIT_CMD[] = "QUIT\r\n";
    write(fd, QUIT_CMD, strlen(QUIT_CMD));
    echo_response(fd);
}
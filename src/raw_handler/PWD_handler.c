#include "context_data.h"
#include "ftptool.h"
#include <string.h>
#include <unistd.h>

void PWD_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;

    char PWD_BUF[] = "PWD\r\n";
    write(fd, PWD_BUF, strlen(PWD_BUF));
    echo_response(fd);
}
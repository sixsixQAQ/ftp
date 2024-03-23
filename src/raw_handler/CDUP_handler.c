#include "context_data.h"
#include "ftptool.h"
#include <string.h>
#include <unistd.h>

void CDUP_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;
    char CDUP_CMD[] = "CDUP\r\n";
    write(fd, CDUP_CMD, strlen(CDUP_CMD));
    echo_response(fd);
}
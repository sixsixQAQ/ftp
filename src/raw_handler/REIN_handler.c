#include "context_data.h"
#include "ftptool.h"
#include <string.h>
#include <unistd.h>

void REIN_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;
    char REIN_CMD[] = "REIN\r\n";
    write(fd, REIN_CMD, strlen(REIN_CMD));
    echo_response(fd);
}
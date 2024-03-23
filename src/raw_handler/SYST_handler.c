#include "context_data.h"
#include "ftptool.h"
#include <string.h>
#include <unistd.h>

void SYST_handler(int argc, const char **argv)
{
    int fd = get_shared_context()->control_connection_fd;
    char SYST_CMD[] = "SYST\r\n";
    write(fd, SYST_CMD, strlen(SYST_CMD));
    echo_response(fd);
}
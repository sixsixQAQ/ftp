#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock.h>
#include <winsock2.h>
#endif

#include "protocol_config.h"

static void usage_open()
{
    printf("Usage open <host> [port]");
}


/**
 * open <host> [port]
 */
void open_handler(int argc, char **argv)
{
    u_short port = default_server_control_connection_port;
    char ip[sizeof("XXX.XXX.XXX.XXX")];
    
    if (argc == 2)
    {
    }
    else if (argc == 3)
    {
        port = atoi(argv[2]);
        if (port < 1 || port > 65535)
        {
            printf("Invalid port: %d\n", port);
            return usage_open();
        }
    }
    else
    {
        return usage_open();
    }

    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(port);
    server_addr.sin_family=AF_INET;
    
}

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <arpa/inet.h>
#include <sys/types.h>
#endif

#include "protocol_config.h"

static void usage_open()
{
    printf("Usage open <host> [port]\n");
}

/**
 * open <host> [port]
 */
void open_handler(int argc, char **argv)
{
    u_short port;
    char ip[sizeof("XXX.XXX.XXX.XXX")];

    if (argc != 2 && argc != 3)
        return usage_open();

    if (argc == 2)
    {
        strncpy(ip, argv[2], sizeof(ip));
        port = default_server_control_connection_port;
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

    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = 
    inet_pto
}

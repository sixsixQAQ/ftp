#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <err.h>
#include <string.h>

#include "context_data.h"

#ifdef _WIN32
#include <winsock2.h>
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
    uint16_t port;
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
    inet_pton(AF_INET,ip, &server_addr.sin_addr.s_addr);

    int fd = socket(AF_INET,SOCK_STREAM, 0);
    if(fd == -1) {
        err(-1, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    if(connect(fd, (struct sockaddr*)&server_addr,sizeof(server_addr)) == -1) {
        err(-1, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    get_shared_context()->is_control_connection_on  = true;
    get_shared_context()->control_connection_fd = fd;

}

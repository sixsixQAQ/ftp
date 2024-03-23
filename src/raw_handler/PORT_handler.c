#include "context_data.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void PORT_handler(int argc, const char **argv)
{
    // todo:验证主动模式
    if (get_shared_context()->is_control_connection_on == false)
    {
        return;
    }
    int fd = get_shared_context()->control_connection_fd;

    char self_ip[IPV4_MAX_SIZE];
    strncpy(self_ip, get_shared_context()->self_ip, sizeof(self_ip));
    for (int i = 0; i < self_ip[i]; ++i)
    {
        if (self_ip[i] == '.')
            self_ip[i] = ',';
    }
    uint16_t port = 65530;
    uint8_t highByte = ((uint8_t *)&port)[1];
    uint8_t lowByte = ((uint8_t *)&port)[0];

    char port_cmd[1024];
    snprintf(port_cmd, sizeof(port_cmd), "PORT %s,%u,%u\r\n", self_ip, highByte, lowByte);
    write(fd, port_cmd, strlen(port_cmd));
}
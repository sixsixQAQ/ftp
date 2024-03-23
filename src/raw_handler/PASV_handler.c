#include "context_data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ftptool.h"
void PASV_handler(int argc, const char **argv)
{
    const char PASV_CMD[] = "PASV\r\n";

    int fd = get_shared_context()->control_connection_fd;
    write(fd, PASV_CMD, strlen(PASV_CMD));

    get_shared_context()->is_passive_mode = true;

    echo_response(fd);

    char last_response[RESPONSE_MAX_SIZE];
    strncpy(last_response, get_shared_context()->last_response, sizeof(last_response));

    char *tailByteBeg = strrchr(last_response, ',') + 1;
    uint8_t tailByte;
    sscanf(tailByteBeg, "%hhu", &tailByte);
    tailByteBeg[-1] = '\0';

    char *highByteBeg = strrchr(last_response, ',') + 1;
    uint8_t headByte;
    sscanf(highByteBeg, "%hhu", &headByte);
    highByteBeg[-1] = '\0';

    uint16_t port;
    ((uint8_t *)&port)[1] = headByte;
    ((uint8_t *)&port)[0] = tailByte;

    char *ip = strchr(last_response, '(') + 1;
    for (int i = 0; ip[i] != '\0'; ++i)
    {
        if (ip[i] == ',')
            ip[i] = '.';
    }

    get_shared_context()->PASV_port = port;
    strncpy(get_shared_context()->PASV_ip, ip, sizeof(get_shared_context()->PASV_ip));
}
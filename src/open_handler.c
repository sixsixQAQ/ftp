#define _XOPEN_SOURCE 700

#include "context_data.h"
#include "ftplimits.h"
#include "ftptool.h"
#include "protocol_config.h"
#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>

static void usage_open()
{
    printf("Usage open <host> [port]\n");
}

static int get_ipv4_by_domain(const char *domain, char *buf, size_t size)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    struct addrinfo *ai_list;
    int ret = getaddrinfo(domain, NULL, &hints, &ai_list);

    if (ret != 0)
    {
        snprintf(buf, size, gai_strerror(ret));
        return -1;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)ai_list->ai_addr;

    if (inet_ntop(AF_INET, &addr->sin_addr, buf, size) == NULL)
    {
        perror("inet_ntop()");
        return -1;
    }
    return 0;
}

/**
 * open <host> [port]
 */
void open_handler(int argc, char **argv)
{
    uint16_t port;
    char domain[DOMAIN_MAX_SIZE];
    char ip[DOMAIN_MAX_SIZE];
    if (argc != 2 && argc != 3)
        return usage_open();

    if (argc == 2)
    {
        strncpy(domain, argv[1], sizeof(domain));
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

    get_ipv4_by_domain(domain, ip, sizeof(ip));

    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        err(-1, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        err(-1, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    { // RESPONSE "220 (vsFTPd 3.0.3)\r\n"
        echo_response(fd);
    }
    { // Send "OPTS UTF8 ON\r\n"
        char buf[1024] = "OPTS UTF8 ON\r\n";
        write(fd, buf, strlen(buf));
    }
    { // Send "200 Always in UTF8 mode.\r\n"
        echo_response(fd);
    }
    { // Send "USER xxxxxxxx\r\n"
        printf("Username(%s):", domain);
        char username[1024];
        fgets(username, sizeof(username), stdin);
        if (username[strlen(username) - 1] == '\n')
        {
            username[strlen(username) - 1] = '\0';
        }
        char USER_CMD[4096];
        snprintf(USER_CMD, sizeof(USER_CMD), "USER %s\r\n", username);
        write(fd, USER_CMD, strlen(USER_CMD));
    }
    { // RESPONSE "331 Please specify the password.\r\n"
        echo_response(fd);
    }
    { // Send "PASS xxxxxx\r\n"
        printf("Password:");
        char password[1024];
        fgets(password, sizeof(password), stdin);
        if (password[strlen(password) - 1] == '\n')
        {
            password[strlen(password) - 1] = '\0';
        }
        char PASS_CMD[4096];
        snprintf(PASS_CMD, sizeof(PASS_CMD), "PASS %s\r\n", password);
        write(fd, PASS_CMD, strlen(PASS_CMD));
    }
    { // RESPONSE "230 Login successful."
        echo_response(fd);
    }

    //剩下步骤不是必须的，可选。

    { // SEND "SYST\r\n"
        const char *SYST_CMD = "SYST\r\n";
        write(fd, SYST_CMD, strlen(SYST_CMD));
    }
    { // RESPONSE "215 UNIC Type: L8\r\n"
        echo_response(fd);
    }

    {
        struct sockaddr_in self_conn_addr;
        socklen_t socklen;

        memset(get_shared_context(), 0, sizeof(context_data));
        get_shared_context()->is_control_connection_on = true;
        get_shared_context()->control_connection_fd = fd;

        getsockname(fd, (struct sockaddr *)&self_conn_addr, &socklen);
        inet_ntop(AF_INET, &self_conn_addr.sin_addr, get_shared_context()->self_ip, socklen);
    }
    return;
}
#ifndef CONTEXT_DATA_H
#define CONTEXT_DATA_H

#include <stdbool.h>
#include <stdint.h>
#include "ftplimits.h"

typedef struct context_data
{

    bool is_data_connection_on;
    bool is_control_connection_on;
    int data_connection_fd;
    int control_connection_fd;

    bool is_passive_mode;
    char self_ip[IPV4_MAX_SIZE];
    uint16_t data_connection_port;

    char last_response[RESPONSE_MAX_SIZE];
    char PASV_ip[IPV4_MAX_SIZE];
    uint16_t PASV_port;
} context_data;

context_data *get_shared_context();

#endif
#ifndef CONTEXT_DATA_H
#define CONTEXT_DATA_H

#include <stdbool.h>

typedef struct context_data
{

    bool is_data_connection_on;
    bool is_control_connection_on;
    int data_connection_fd;
    int control_connection_fd;
} context_data;

context_data *get_shared_context();

#endif
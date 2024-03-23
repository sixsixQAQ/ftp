#include "context_data.h"

context_data *get_shared_context()
{
    static context_data data;
    return &data;
}

__attribute__((constructor))
void init_context_data()
{
    context_data* data = get_shared_context();
    data->is_control_connection_on=false;
    data->is_data_connection_on=false;
}
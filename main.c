#include <stdio.h>
#include <string.h>
#include "cmd.h"

#define MAX_LINE 1024
#define MAX_ARGC 50

void execute(int argc,const char **argv)
{
    const cmd *c = get_cmd_info(argv[0]);
    if (c == NULL)
    {
        printf("Invalid Command.\n");
        return;
    }
    else
    {
        c->handler(argc, argv);
    }
}

int main(int argc, char **argv)
{
    char buf[MAX_LINE];

    int arg_count;
    char args[MAX_ARGC][MAX_LINE];
    const char *arg_vector[MAX_ARGC];

    for (int i = 0; i < MAX_ARGC; ++i)
        arg_vector[i] = args[i];

    const char delim[] = " \n";

    printf("ftp>");
    for (; fgets(buf, sizeof(buf), stdin);)
    {
        char *token = strtok(buf, delim);
        if (token == NULL)
            continue;
        else
        {
            strncpy(args[0], token, sizeof(args[0]));
            arg_count = 1;
        }

        for (; arg_count < MAX_ARGC; ++arg_count)
        {
            token = strtok(NULL, delim);
            if (token == NULL)
                break;
            else
                strncpy(args[arg_count], token, sizeof(args[arg_count]));
        }

        execute(arg_count, arg_vector);
        printf("ftp>");
    }

    return 0;
}


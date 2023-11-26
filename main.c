#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_LINE 1024
#define MAX_ARGC 50

#define MAX_HELP_TEXT_SIZE 1024
#define MAX_CMD_NAME_SIZE 20
#define DEFAULT_HELP_TEXT ""
#define countof(arr) (sizeof(arr) / sizeof(arr[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct cmd
{
    char std_name[MAX_CMD_NAME_SIZE];
    char name[MAX_CMD_NAME_SIZE];
    void (*handler)(int argc, char **argv);
} cmd;

void default_cmd_handler(int argc, char **argv);

static const cmd g_cmd_tables[] = {
    {"USER", "user", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"QUIT", "quit", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"PORT", "port", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"TYPE", "type", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"MODE", "mode", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"STRU", "struct", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"RETR", "get", DEFAULT_HELP_TEXT, default_cmd_handler},
    {"STOR", "put", DEFAULT_HELP_TEXT, default_cmd_handler}};

size_t get_cmd_count()
{
    return countof(g_cmd_tables);
}

const cmd *get_cmd_info(const char *cmd_name)
{
    for (int i = 0; i < countof(g_cmd_tables); ++i)
    {
        if (strncmp(cmd_name, g_cmd_tables[i].name,
                    MIN(strlen(cmd_name), strlen(g_cmd_tables[i].name))) == 0)
            return &g_cmd_tables[i];
    }
    return NULL;
}

void execute(int argc, char (*argv)[MAX_LINE])
{
    const cmd *c = get_cmd_info(argv[0]);
    if (c == NULL)
        return;
    else
    {
        c->handler(argc, argv);
    }
}

int main(int argc, char **argv)
{
    char buf[MAX_LINE];

    int arg_count;
    char args_vector[MAX_ARGC][MAX_LINE];
    const char delim[] = " \n";

    for (; fgets(buf, sizeof(buf), stdin);)
    {
        char *token = strtok(buf, delim);
        if (token == NULL)
            continue;
        else
        {
            strncpy(args_vector[0], token, sizeof(args_vector[0]));
            arg_count = 1;
        }

        for (; arg_count < MAX_ARGC; ++arg_count)
        {
            token = strtok(NULL, delim);
            if (token == NULL)
                break;
            else
                strncpy(args_vector[arg_count], token, sizeof(args_vector[arg_count]));
        }

        execute(arg_count, args_vector);
    }

    return 0;
}

void default_cmd_handler(int argc, char **argv)
{
    printf("Command <%s> haven't be implemented.\n", argv[0]);
}
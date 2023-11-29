#include "cmd.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "help_handler.h"

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void default_cmd_handler (int argc, char **argv)
{
	printf ("Command '%s' haven't be implemented.\n", argv[0]);
}

const cmd g_cmd_tables[] =
{
	{"HELP", "help", "print local help information", help_handler},
	{"USER", "user", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"QUIT", "quit", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"PORT", "port", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"TYPE", "type", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"MODE", "mode", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"STRU", "struct", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"RETR", "get", DEFAULT_HELP_TEXT, default_cmd_handler},
	{"STOR", "put", DEFAULT_HELP_TEXT, default_cmd_handler}
};

size_t get_cmd_count()
{
	return countof (g_cmd_tables);
}



const cmd *get_cmd_info (const char *cmd_name)
{
	for (int i = 0; i < countof (g_cmd_tables); ++i)
	{
		if (strncmp (cmd_name, g_cmd_tables[i].name,
		             MIN (strlen (cmd_name), strlen (g_cmd_tables[i].name))) == 0)
			return &g_cmd_tables[i];
	}
	return NULL;
}
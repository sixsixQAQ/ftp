#ifndef CMD_H
#define CMD_H

#include <stddef.h>

#define DEFAULT_HELP_TEXT "TODO"

typedef struct cmd
{
	const char *raw_name;
	const char *name;
	const char *help;
	void (*handler) (int argc, const char **argv);
} cmd;

size_t get_cmd_count();
const cmd *get_cmd_info (const char *cmd_name);

#endif
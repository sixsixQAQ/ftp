#include "cmd.h"
#include <stdio.h>

extern const cmd g_cmd_tables[];

void help_handler (int argc, char **argv)
{
	if (argc == 1) //打印命令列表
	{
		printf ("Usage:help <command>.  Commands are:");
		for (int i = 0; i < get_cmd_count(); ++i)
		{
			printf ("%s\t", g_cmd_tables[i].name);
			if (i % 5 == 0)
				putchar ('\n');
		}
	}
	else // 打印命令帮助信息
	{
		for (int i = 1 ; i < argc; ++i)
		{
			printf ("%s\t%s\n", argv[i], get_cmd_info (argv[i])->help);
		}
	}
}
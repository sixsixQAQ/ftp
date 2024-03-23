#include "cmd.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "open_handler.h"
#include "raw_handler.h"

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// static void default_cmd_handler(int argc, const char **argv)
// {
//     printf("Command '%s' haven't be implemented.\n", argv[0]);
// }

void help_handler(int argc, const char **argv);
const cmd g_cmd_tables[] = {
    {"open", "open", "open <ip> [port]", open_handler},
    {"help", "help", "help [command]", help_handler},
    {"USER", "USER", "USER <SP> <username> <CRLF>", USER_handler},
    {"PASS", "PASS", "PASS <SP> <password> <CRLF>", PASS_handler},
    {"ACCT", "ACCT", "ACCT <SP> <account-information> <CRLF>", ACCT_handler},
    {"CWD", "CWD", "CWD <SP> <pathname> <CRLF>", CWD_handler},
    {"CDUP", "CDUP", "CDUP <CRLF>", CDUP_handler},
    {"SMNT", "SMNT", "SMNT <SP> <pathname> <CRLF>", SMNT_handler},
    {"QUIT", "QUIT", "QUIT <CRLF>", QUIT_handler},
    {"REIN", "REIN", "REIN <CRLF>", REIN_handler},
    {"PORT", "PORT", "PORT <SP> <host-port> <CRLF>", PORT_handler},
    {"PASV", "PASV", "PASV <CRLF>", PASV_handler},
    {"TYPE", "TYPE", "TYPE <SP> <type-code> <CRLF>", TYPE_handler},
    {"STRU", "STRU", "STRU <SP> <structure-code> <CRLF>", STRU_handler},
    {"MODE", "MODE", "MODE <SP> <mode-code> <CRLF>", MODE_handler},
    {"RETR", "RETR", "RETR <SP> <pathname> <CRLF>", RETR_handler},
    {"STOR", "STOR", "STOR <SP> <pathname> <CRLF>", STOR_handler},
    {"STOU", "STOU", "STOU <CRLF>", STOU_handler},
    {"APPE", "APPE", "APPE <SP> <pathname> <CRLF>", APPE_handler},
    {"ALLO", "ALLO", "ALLO <SP> <decimal-integer> [<SP> R <SP> <decimal-integer>] <CRLF>", ALLO_handler},
    {"REST", "REST", "REST <SP> <marker> <CRLF>", REST_handler},
    {"RNFR", "RNFR", "RNFR <SP> <pathname> <CRLF>", RNFR_handler},
    {"RNTO", "RNTO", "RNTO <SP> <pathname> <CRLF>", RNTO_handler},
    {"ABOR", "ABOR", "ABOR <CRLF>", ABOR_handler},
    {"DELE", "DELE", "DELE <SP> <pathname> <CRLF>", DELE_handler},
    {"RMD", "RMD", "RMD <SP> <pathname> <CRLF>", RMD_handler},
    {"MKD", "MKD", "MKD <SP> <pathname> <CRLF>", MKD_handler},
    {"PWD", "PWD", "PWD <CRLF>", PWD_handler},
    {"LIST", "LIST", "LIST [<SP> <pathname>] <CRLF>", LIST_handler},
    {"NLST", "NLST", "NLST [<SP> <pathname>] <CRLF>", NLST_handler},
    {"SITE", "SITE", "SITE <SP> <string> <CRLF>", SITE_handler},
    {"SYST", "SYST", "SYST <CRLF>", SYST_handler},
    {"STAT", "STAT", "STAT [<SP> <pathname>] <CRLF>", STAT_handler},
    {"HELP", "HELP", "HELP [<SP> <string>] <CRLF>", HELP_handler},
    {"NOOP", "NOOP", "NOOP <CRLF>", NOOP_handler},
};

size_t get_cmd_count()
{
    return countof(g_cmd_tables);
}

const cmd *get_cmd_info(const char *cmd_name)
{
    for (int i = 0; i < countof(g_cmd_tables); ++i)
    {
        if (strncmp(cmd_name, g_cmd_tables[i].name, MIN(strlen(cmd_name), strlen(g_cmd_tables[i].name))) == 0)
            return &g_cmd_tables[i];
    }
    return NULL;
}

void help_handler(int argc, const char **argv)
{
    if (argc == 1) // 打印命令列表
    {
        printf("Commands may be abbreviated.  Commands are:\n");
        for (int i = 0; i < get_cmd_count(); ++i)
        {
            printf("%s\t\t", g_cmd_tables[i].name);
            if ((i + 1) % 5 == 0)
                putchar('\n');
        }
        putchar('\n');
    }
    else // 打印命令帮助信息
    {
        for (int i = 1; i < argc; ++i)
        {
            printf("%s\t\t%s\n", argv[i], get_cmd_info(argv[i])->help);
        }
    }
}
#ifndef HANDLER_H
#define HANDLER_H

void USER_handler(int argc, const char **argv);
void PASS_handler(int argc, const char **argv);
void ACCT_handler(int argc, const char **argv);
void CWD_handler(int argc, const char **argv);
void CDUP_handler(int argc, const char **argv);
void SMNT_handler(int argc, const char **argv);
void QUIT_handler(int argc, const char **argv);
void REIN_handler(int argc, const char **argv);
void PORT_handler(int argc, const char **argv);
void PASV_handler(int argc, const char **argv);
void TYPE_handler(int argc, const char **argv);
void STRU_handler(int argc, const char **argv);
void MODE_handler(int argc, const char **argv);
void RETR_handler(int argc, const char **argv);
void STOR_handler(int argc, const char **argv);
void STOU_handler(int argc, const char **argv);
void APPE_handler(int argc, const char **argv);
void ALLO_handler(int argc, const char **argv);
void REST_handler(int argc, const char **argv);
void RNFR_handler(int argc, const char **argv);
void RNTO_handler(int argc, const char **argv);
void ABOR_handler(int argc, const char **argv);
void DELE_handler(int argc, const char **argv);
void RMD_handler(int argc, const char **argv);
void MKD_handler(int argc, const char **argv);
void PWD_handler(int argc, const char **argv);
void LIST_handler(int argc, const char **argv);
void NLST_handler(int argc, const char **argv);
void SITE_handler(int argc, const char **argv);
void SYST_handler(int argc, const char **argv);
void STAT_handler(int argc, const char **argv);
void HELP_handler(int argc, const char **argv);
void NOOP_handler(int argc, const char **argv);

#endif
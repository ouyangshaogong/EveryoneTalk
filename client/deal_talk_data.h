#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include "common.h"

void init_talk_lists(string f_name);
void talk_lists_down(bool is_get_msg);
void talk_lists_up();
void fill_talk_data();
void get_send_msg(string my_name, string f_name);
int send_msg_to_server(char *c_msg, string f_name);
void recvmsg_from_server(string str_msg);



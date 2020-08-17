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
void get_send_msg(string my_name);
void fill_talk_data();



#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include "common.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void init_friend_lists(vector<friend_list> &f_lists);
void friend_lists_down(vector<friend_list> &f_lists);
void friend_lists_up(vector<friend_list> &f_lists);

void friend_lists_modify(string user_name);

void friend_lists_query();

void friend_lists_show();

void friend_lists_switch();

void friend_lists_quit(string user_name, bool &is_quit);

void friend_lists_help();

bool is_user_exist(vector<friend_list> &f_lists, int user_id, string &f_name);

void fill_friend_data();

void init_user_state();

void deal_login_data(bool &is_quit);

void modify_friend_state(string user_name, user_state state);


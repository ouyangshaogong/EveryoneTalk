#ifndef __EVERYONE_TALK__
#define __EVERYONE_TALK__

#include <stdio.h>	
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include "common.h"
#include "serialize_msg.h"

using namespace std;

int register_to_server(UserInfo user_info);
int login(LoginInfo login_info);
int get_friend_list(string user_name, vector<friend_list> &f_lists);
int change_user_state(string strMsg, user_state state);
int quit_from_server(string user_name);
#endif
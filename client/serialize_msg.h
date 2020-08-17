#ifndef __SERIALIZE_MSG__
#define __SERIALIZE_MSG__

#include <stdio.h>	
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include "common.h"

using namespace std;

void register_serialize(UserInfo user_info, int register_id, char *pStr);
void login_serialize(LoginInfo login_info, int login_id, char *pStr);
void friend_list_serialize(int friend_list_id, string &user_name, char *pStr);
void friend_list_deserialize(string respond, vector<friend_list> &f_lists);
void friend_online_state_serialize(int online_state_id, char *pStr);
void friend_online_state_deserialize(string strNotifyData, string &user_name, user_state &state);
void serialize_user_state(int change_id, string user_name, user_state state, char *pStr);
void sendmsg_serialize(char *c_msg, int MSG_ID_EVERYONETALK_SEND_MSG, char *request);
void serialize_quit(int change_id, string user_name, char *pStr);

#endif
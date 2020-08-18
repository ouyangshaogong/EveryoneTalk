#include "serialize_msg.h"


void register_serialize(UserInfo user_info, int register_id, char *pStr)
{
    sprintf(pStr, "%d;%s;%s;%s;%s;%s\n", register_id, user_info.user_name, user_info.passwd, 
        user_info.sex, user_info.birth, user_info.hometown);
}

void login_serialize(LoginInfo login_info, int login_id, char *pStr)
{
    sprintf(pStr, "%d;%s;%s;%s;%d\n", login_id, login_info.user_name.c_str(), 
        login_info.passwd.c_str(), login_info.ip.c_str(), login_info.port);
}

void friend_list_serialize(int friend_list_id, string &user_name, char *pStr)
{
    sprintf(pStr, "%d;%s\n", friend_list_id, user_name.c_str());
}

void serialize_user_state(int change_id, string user_name, user_state state, char *pStr)
{
    sprintf(pStr, "%d;%s;%d\n", change_id, user_name.c_str(), state);
}

void serialize_quit(int change_id, string user_name, char *pStr)
{
    sprintf(pStr, "%d;%s\n", change_id, user_name.c_str());
}


void friend_list_deserialize(string respond, vector<friend_list> &f_lists)
{
    // 1,James,on;2,Bob,on;
    int pos = 0;
    string str_friend;
    friend_list f_list;
    pos = respond.find(";");
    while (pos != string::npos)
    {
        str_friend = respond.substr(0, pos);
        respond = respond.substr(pos + 1);

        pos = str_friend.find(",");
        f_list.id = atoi(str_friend.substr(0, pos).c_str());
        str_friend = str_friend.substr(pos + 1);

        pos = str_friend.find(",");
        f_list.user_name = str_friend.substr(0, pos);
        f_list.state = (user_state)atoi(str_friend.substr(pos + 1).c_str());

        f_lists.push_back(f_list);

        pos = respond.find(";");
    }
}

void friend_online_state_serialize(int online_state_id, char *pStr)
{
    sprintf(pStr, "%d", online_state_id);
}

void friend_online_state_deserialize(string strNotifyData, string &user_name, user_state &state)
{
    int pos = 0;
    pos = strNotifyData.find(";");
    if (pos != string::npos)
    {
        user_name = strNotifyData.substr(0, pos);
        state = (user_state)atoi(strNotifyData.substr(pos + 1).c_str());
    }
}


void sendmsg_serialize(char *c_msg, int change_id, string f_name, char *request)
{
    sprintf(request, "%d;%s;%s\n", change_id, f_name.c_str(), c_msg);
}

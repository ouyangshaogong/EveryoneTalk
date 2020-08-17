#include "everyone_talk.h"


extern int deal_msg(char *pMsg, char *pRet, int nlength);


int register_to_server(UserInfo user_info)
{
    // 1.将注册信息序列化到消息中
    char request[MAXLINE] = {0};
    register_serialize(user_info, MSG_ID_EVERYONETALK_REGISTER, request);
    // 2.发送消息，并返回结果
    
    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);
    if (ret == OK) {
        ret = atoi(respond);
    }

    return ret;
}

int login(LoginInfo login_info)
{
    char request[MAXLINE] = {0};
    login_serialize(login_info, MSG_ID_EVERYONETALK_LOGIN, request);

    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);
    if (ret == OK)
    {
        ret = atoi(respond);
    }

    return ret;
}

int get_friend_list(string user_name, vector<friend_list> &f_lists)
{
    char request[MAXLINE] = {0};
    friend_list_serialize(MSG_ID_EVERYONETALK_FRIEND_LIST, user_name, request);

    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);

    syslog(LOG_ERR, "respond:%s", respond);
    if (ret == OK)
    {
        friend_list_deserialize(respond, f_lists);
        return OK;
    }
    else
    {
        return FAIL;
    }
}



int change_user_state(string strMsg, user_state state)
{
    // user_name:state
    char request[MAXLINE] = {0};
    serialize_user_state(MSG_ID_EVERYONETALK_CHANGE_USER_STATE, strMsg, state, request);

    syslog(LOG_ERR, "AAAA client change_user_state. user_name:%s", strMsg.c_str());
    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);
    if (ret == OK)
    {
        ret = atoi(respond);
    }

    return ret;
}

int quit_from_server(string user_name)
{
    // user_name:state
    char request[MAXLINE] = {0};
    serialize_quit(MSG_ID_EVERYONETALK_QUIT, user_name, request);

    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);
    if (ret == OK)
    {
        ret = atoi(respond);
    }

    return ret;
}






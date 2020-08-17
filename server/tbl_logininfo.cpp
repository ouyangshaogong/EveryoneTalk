#include "tbl_logininfo.h"

vector<tbl_login> g_vecTableLogin;


void init_tbl_login()
{
    
}

void insert_login_info(tbl_login l_info)
{
    g_vecTableLogin.push_back(l_info);

    string strSQL;
    char c_sql[MAXLINE] = {0};
    strSQL = "INSERT INTO login_info(user_name,c_ip,c_port,login_time,state,user_id) VALUES (";
    sprintf(c_sql, "'%s','%s',%d, '%s',%d,%d)", l_info.user_name.c_str(), 
        l_info.ip.c_str(), l_info.port, l_info.login_time.c_str(), l_info.state, l_info.user_id);
    strSQL += c_sql;
    
    insert_mysql(strSQL);
}

void get_logininfo_by_username(string user_name, tbl_login &t_login)
{
    for (int i = 0; i < g_vecTableLogin.size(); i++)
    {
        if (g_vecTableLogin[i].user_name == user_name)
        {
            t_login.user_id = g_vecTableLogin[i].user_id;
            t_login.user_name = g_vecTableLogin[i].user_name;
            t_login.state = g_vecTableLogin[i].state;
            break;
        }
    }
}

void get_friendstates_by_userid(vector<int> &userids, map<int, user_state> &userid_map_states)
{
    for (int i = 0; i < userids.size(); i++)
    {
        for (int j = 0; j < g_vecTableLogin.size(); j++)
        {
            if (userids[i] == g_vecTableLogin[j].id)
            {
                userid_map_states.insert(map<int, user_state>::value_type(g_vecTableLogin[j].id, g_vecTableLogin[j].state));
                break;
            }
        }
    }
}

void get_logininfos_by_userid(vector<int> &userids, vector<tbl_login> &t_logins)
{
    for (int i = 0; i < userids.size(); i++)
    {
        for (int j = 0; j < g_vecTableLogin.size(); j++)
        {
            if (userids[i] == g_vecTableLogin[j].user_id)
            {
                t_logins.push_back(g_vecTableLogin[j]);
                break;
            }
        }
    }
}


int update_login_state(       string user_name, user_state state)
{
    syslog(LOG_ERR, "user_name:%s, state:%d, g_vecTableLogin.size():%d\n", 
        user_name.c_str(), state, g_vecTableLogin.size());
    string login_time;
    for (int j = 0; j < g_vecTableLogin.size(); j++)
    {
        syslog(LOG_ERR, "g_vecTableLogin[j].user_name:%s, g_vecTableLogin[j].state:%d\n", 
            g_vecTableLogin[j].user_name.c_str(), g_vecTableLogin[j].state);
        if (user_name == g_vecTableLogin[j].user_name)
        {
            g_vecTableLogin[j].state = state;
            login_time = g_vecTableLogin[j].login_time;
            break;
        }
    }

    char c_state[10] = {0};
    sprintf(c_state, "%d", state);
    
    string strSQL = "UPDATE login_info SET state=";
    strSQL += string(c_state);
    strSQL += " where user_name=";
    strSQL += string("'") + user_name.c_str() + string("'");
    strSQL += " and login_time=";
    strSQL += string("'") + login_time.c_str() + string("'");
    syslog(LOG_ERR, "strSQL:%s\n", strSQL.c_str());
    
    return update_mysql(strSQL);
}

int update_quit_state(string user_name, user_state state)
{
    syslog(LOG_ERR, "user_name:%s, state:%d, g_vecTableLogin.size():%d\n", 
        user_name.c_str(), state, g_vecTableLogin.size());
    //删除缓存
    string login_time;
    vector<tbl_login>::iterator iter = g_vecTableLogin.begin();
    for(; iter != g_vecTableLogin.end();)
    {
        if (iter->user_name == user_name)
        {
            login_time = iter->login_time;
            iter = g_vecTableLogin.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    //更新数据库
    char c_state[10] = {0};
    sprintf(c_state, "%d", state);
    
    string strSQL = "UPDATE login_info SET state=";
    strSQL += string(c_state);
    strSQL += " where user_name=";
    strSQL += string("'") + user_name.c_str() + string("'");
    strSQL += " and login_time=";
    strSQL += string("'") + login_time.c_str() + string("'");
    syslog(LOG_ERR, "strSQL:%s\n", strSQL.c_str());
    
    return update_mysql(strSQL);
}

#include "execute_sql.h"

//数据库结构信息
struct tbl_login
{
    int id;
    string user_name;
    string ip;
    int port;
    string login_time;
    user_state state;
    int user_id;
};

void init_tbl_login();
void get_logininfos_by_userid(vector<int> &userids, vector<tbl_login> &t_logins);
void insert_login_info(tbl_login l_info);
void get_logininfo_by_username(string user_name, tbl_login &t_login);
int update_login_state(       string user_name, user_state state);
void get_friendstates_by_userid(vector<int> &userids, map<int, user_state> &userid_map_states);
int update_quit_state(string user_name, user_state state);



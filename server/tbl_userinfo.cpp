#include "tbl_userinfo.h"

vector<tbl_User> g_vecTableUSer;


void init_tbl_user()
{
    string strSQL = "select * from user_info";
    vector < map < string, string > > tables_data;
    select_mysql(strSQL, tables_data);

    for (int i = 0; i < tables_data.size(); i++)
    {
        tbl_User t_user;
        map<string, string>::iterator iter = tables_data[i].begin();
        for (;iter != tables_data[i].end(); ++iter)
        {
            
            if (iter->first == "id")
            {
                t_user.id = atoi(iter->second.c_str());
            }

            if (iter->first == "username")
            {
                t_user.user_name = iter->second;
            }

            if (iter->first == "passwd")
            {
                t_user.passwd = iter->second;
            }

            if (iter->first == "sex")
            {
                t_user.sex = iter->second;
            }

            if (iter->first == "birth")
            {
                t_user.birth = iter->second;
            }

            if (iter->first == "hometown")
            {
                t_user.hometown = iter->second;
            }
            
        }

        g_vecTableUSer.push_back(t_user);
    }
}

vector<tbl_User> get_user_nfo()
{
    return g_vecTableUSer;
}

void insert_user_info(UserInfo user_info)
{
    tbl_User t_user;
    t_user.user_name = user_info.user_name;
    t_user.passwd = user_info.passwd;
    t_user.sex = user_info.sex;
    t_user.birth = user_info.birth;
    t_user.hometown = user_info.hometown;
    g_vecTableUSer.push_back(t_user);
    

    string strSQL;
    char c_sql[MAXLINE] = {0};
    strSQL = "INSERT INTO user_info(username,passwd,sex,birth,hometown) VALUES (";
    sprintf(c_sql, "%s,%s,%s,%s,%s", user_info.user_name, 
        user_info.passwd, user_info.sex, user_info.birth, user_info.hometown);
    strSQL += c_sql;
    
    insert_mysql(strSQL);
}

void get_friendinfo_by_userid(vector<int> &userids, vector<tbl_User> &t_users)
{
    for (int i = 0; i < userids.size(); i++)
    {
        for(int j = 0; j < g_vecTableUSer.size(); j++)
        {
            if (userids[i] == g_vecTableUSer[j].id)
            {
                t_users.push_back(g_vecTableUSer[j]);
                break;
            }
        }
    }
}

int get_userid_by_username(string user_name)
{
    for(int j = 0; j < g_vecTableUSer.size(); j++)
    {
        if (user_name == g_vecTableUSer[j].user_name)
        {
            return g_vecTableUSer[j].id;
        }
    }
}



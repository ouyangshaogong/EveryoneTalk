#include "tbl_friendinfo.h"

vector<tbl_friend> g_vecTableFriend;


void init_tbl_friend()
{
    string strSQL = "select * from friend_info";
    vector <map<string, string> > tables_data;
    select_mysql(strSQL, tables_data);

    for (int i = 0; i < tables_data.size(); i++)
    {
        tbl_friend t_friend;
        map<string, string>::iterator iter = tables_data[i].begin();
        for (;iter != tables_data[i].end(); ++iter)
        {
            
            if (iter->first == "id")
            {
                t_friend.id = atoi(iter->second.c_str());
            }

            if (iter->first == "user_id")
            {
                t_friend.user_id = atoi(iter->second.c_str());
            }

            if (iter->first == "friend_id")
            {
                t_friend.friend_id = atoi(iter->second.c_str());
            }

        }

        g_vecTableFriend.push_back(t_friend);
    }
}

void insert_friend_info(tbl_friend t_info)
{
   
}

void get_friendids_by_userid(int user_id, vector<int> &userids)
{
    for (int i = 0; i < g_vecTableFriend.size(); i++)
    {
        if (user_id == g_vecTableFriend[i].user_id)
        {
            userids.push_back(g_vecTableFriend[i].friend_id);
        }
    }
}





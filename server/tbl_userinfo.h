#include "execute_sql.h"

//数据库结构信息
struct tbl_User
{
    int id;
    string user_name;
    string passwd;
    string sex;
    string birth;
    string hometown;
};

void init_tbl_user();
vector<tbl_User> get_user_nfo();
int get_userid_by_username(string user_name);
void insert_user_info(UserInfo user_info);
void get_friendinfo_by_userid(vector<int> &userids, vector<tbl_User> &t_users);



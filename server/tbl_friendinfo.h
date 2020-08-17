#include "execute_sql.h"

//数据库结构信息
struct tbl_friend
{
    int id;
    int user_id;
    int friend_id;
};

void init_tbl_friend();
void get_friendids_by_userid(int user_id, vector<int> &userids);



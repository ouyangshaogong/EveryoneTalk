#include "deal_friend_data.h"
#include "everyone_talk.h"

#define     SIZE    10

const int FRIEND_SPACE_SIZE = 10;
friend_list print_f_lists[10];
static int g_current_up_pos = 0;
static int g_current_down_pos = 0;

map<string, user_state> g_keyMapState;

extern int g_interface_friend;

void init_friend_lists(vector<friend_list> &f_lists)
{
    int length = MIN(FRIEND_SPACE_SIZE, f_lists.size());

    int i = 0;
    for(; i < length; i++)
    {
        print_f_lists[i].id = f_lists[i].id;
        print_f_lists[i].user_name = f_lists[i].user_name;
        print_f_lists[i].state = f_lists[i].state;
    }

    g_current_up_pos = 0;
    g_current_down_pos = i;
}

void modify_friend_state(string user_name, user_state state)
{
    for(int i = 0; i < FRIEND_SPACE_SIZE; i++)
    {
        if (print_f_lists[i].user_name == user_name)
        {
            print_f_lists[i].state = state;
            break;
        }
    }

    if (g_interface_friend)
    {
        fill_friend_data();
    }
}

void fill_friend_data()
{
    printf("\033[11A");  //上移11位
    for(int i = 0; i < FRIEND_SPACE_SIZE; i++)
    {
        if (print_f_lists[i].id == 0)
        {
            printf("\n");
            continue;
        }
        
        printf("\033[3C"); //右移3位
        printf("%2d", print_f_lists[i].id);
        printf("\033[7C"); //右移3位
        printf("%-10s", print_f_lists[i].user_name.c_str());
        printf("\033[5C"); //右移3位
        printf("%-6d", print_f_lists[i].state);
        printf("\n");
    }

    printf("\n");
}

void friend_lists_down(vector<friend_list> &f_lists)
{
    if (f_lists.size() < FRIEND_SPACE_SIZE)
    {
        return ;
    }
    else
    {
        if (g_current_down_pos >= f_lists.size())
        {
            syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
            return ;
        }

        int i = 0;
        for (; i < FRIEND_SPACE_SIZE - 1; i++)
        {
            print_f_lists[i].id = print_f_lists[i + 1].id;
            print_f_lists[i].user_name = print_f_lists[i + 1].user_name;
            print_f_lists[i].state = print_f_lists[i + 1].state;
        }

        print_f_lists[i].id = f_lists[g_current_down_pos].id;
        print_f_lists[i].user_name = f_lists[g_current_down_pos].user_name;
        print_f_lists[i].state = f_lists[g_current_down_pos].state;

        g_current_up_pos++;
        g_current_down_pos++;

        syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
    }
}

void friend_lists_up(vector<friend_list> &f_lists)
{
    if (f_lists.size() < FRIEND_SPACE_SIZE)
    {
        return ;
    }
    else
    {

        if (g_current_up_pos <= 0)
        {
            syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
            return ;
        }

        g_current_up_pos--;
        g_current_down_pos--;

        int i = FRIEND_SPACE_SIZE - 1;
        for (; i > 0; i--)
        {
            print_f_lists[i].id = print_f_lists[i - 1].id;
            print_f_lists[i].user_name = print_f_lists[i - 1].user_name;
            print_f_lists[i].state = print_f_lists[i - 1].state;
        }

        print_f_lists[i].id = f_lists[g_current_up_pos].id;
        print_f_lists[i].user_name = f_lists[g_current_up_pos].user_name;
        print_f_lists[i].state = f_lists[g_current_up_pos].state;

        syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
    }
}

bool is_user_exist(vector<friend_list> &f_lists, int user_id, string &f_name)
{
    for (int i = 0; i < f_lists.size(); i++)
    {
        if (user_id == f_lists[i].id)
        {
            f_name = f_lists[i].user_name;
            return true;
        }
    }

    return false;
}

void get_user_state(char *c_key)
{
    struct termios old_opt, opt;
    
    tcgetattr(0, &old_opt);
    opt = old_opt;

    opt.c_lflag &= ~ECHO;
    opt.c_lflag &= ~ICANON;

   
    syslog(LOG_ERR, "set attr.");
    while(1)
    {
        tcsetattr(0, TCSANOW, &opt); //设置
        char ch = getchar();
        if (ch == 58)
        {
            tcsetattr(0, TCSANOW, &old_opt); //恢复
            printf(":");
            
            char t_cmd[10];
            int i = 0;
            while ((ch = getchar()) != '\n')
            {
                t_cmd[i] = ch;
                i++;
            }

            t_cmd[i] = '\0';
            strcpy(c_key, t_cmd);
            
            break;
        }
        else 
        {
             tcsetattr(0, TCSANOW, &old_opt); //恢复
        }
    }
}

void init_user_state()
{
    g_keyMapState.insert(map<string, user_state>::value_type(string("ONLINE"), ONLINE));
    g_keyMapState.insert(map<string, user_state>::value_type(string("OFFLINE"), OFFLINE));
    g_keyMapState.insert(map<string, user_state>::value_type(string("LEAVE"), LEAVE));
    g_keyMapState.insert(map<string, user_state>::value_type(string("BUSY"), BUSY));
    g_keyMapState.insert(map<string, user_state>::value_type(string("NOT_DISTURB"), NOT_DISTURB));
    g_keyMapState.insert(map<string, user_state>::value_type(string("ONLINE"), STEALTH));
}

void deal_user_state(char *c_key, user_state &u_state)
{
    map<string, user_state>::iterator iter = g_keyMapState.find(c_key);
    if (iter != g_keyMapState.end())
    {
        u_state = iter->second;
    }
}

void friend_lists_modify(string user_name)
{
    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容

    char c_key[SIZE] = {0};
    user_state u_state;
    get_user_state(c_key);
    deal_user_state(c_key, u_state);
    if (change_user_state(user_name, u_state))
    {
        syslog(LOG_ERR, "modify_state fail.user_name:%s", user_name.c_str());
    }

    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容
}

void friend_lists_query()
{

}

void friend_lists_show()
{
    
}

void friend_lists_switch()
{
    
}

void friend_lists_quit(string user_name, bool &is_quit)
{
    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容
    is_quit = true;
    quit_from_server(user_name);
}






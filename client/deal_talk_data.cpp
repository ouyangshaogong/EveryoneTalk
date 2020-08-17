#include "deal_talk_data.h"
#include "serialize_msg.h"
#include "connect_server.h"

#define TALK_SPACE_SIZE 10
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static vector<string> message_lists;
static string print_talk_lists[TALK_SPACE_SIZE];
static string friend_name;
static int g_current_up_pos = 0;
static int g_current_down_pos = 0;
static int g_current_pos = 0;
void test_init()
{
    for(int i = 0; i < 6; i++)
    {
        char c_i[10];
        sprintf(c_i, "%d", i);
        message_lists.push_back(string("me:hello,world:") + string(c_i));
        //message_lists.push_back(string("friend:hello,world:") + string(c_i));
    }
    
}

void init_talk_lists(string f_name)
{
    //test_init();
    friend_name = f_name;

    if (message_lists.size() == 0)
    {
        g_current_up_pos = 0;
        g_current_down_pos = 0;
        return ;
    }

    int i = 0;
    int j = 0;
    int length = MIN(TALK_SPACE_SIZE, message_lists.size());
    for(i = 0, j = length - 1; i < length ; i++, j--)
    {
        syslog(LOG_ERR, "i:%d, ", i, print_talk_lists[i].c_str());
        syslog(LOG_ERR, "message_lists.size() - j - 1:%d, message_lists[message_lists.size() - j - 1]:%s", 
            message_lists.size() - j - 1, message_lists[message_lists.size() - j - 1].c_str());
        print_talk_lists[i] = message_lists[message_lists.size() - j - 1];
    }

    g_current_up_pos = message_lists.size() - length;
    g_current_down_pos = message_lists.size() - 1;
}

void recvmsg_from_server(string msg)
{
    string str_msg = friend_name + ":" + msg;
    message_lists.push_back(str_msg);
}

int send_msg_to_server(char *c_msg)
{
    char request[MAXLINE] = {0};
    sendmsg_serialize(c_msg, MSG_ID_EVERYONETALK_SEND_MSG, request);

    char respond[MAXLINE] = {0};
    int ret = deal_msg(request, respond, MAXLINE);
    if (ret == OK)
    {
        ret = atoi(respond);
    }

    return ret;
}

void delete_func_key(char *sendline, string strsub)
{
    string send_line = sendline;
    string tmp_line;
    int pos = 0;
    
    while((pos = send_line.find(strsub)) != string::npos)
    {
        tmp_line = send_line.substr(pos + strsub.length());
        send_line = send_line.substr(0, pos);
        send_line += tmp_line;
    }

    strcpy(sendline, send_line.c_str());
}

void get_send_msg(string my_name)
{
    char sendline[MAXLINE] = {0};

    while(1)
    {
        fgets(sendline, MAXLINE, stdin);
        sendline[strlen(sendline) - 1] = '\0';

        if (!strcmp(sendline, ":q"))
        {
            printf("\033[1A");
            printf("\033[K"); //清除从光标到行尾的内容
            break;
        }
        
        delete_func_key(sendline, "\033[A");
        delete_func_key(sendline, "\033[B");
        delete_func_key(sendline, "\033[C");
        delete_func_key(sendline, "\033[D");

        if (strlen(sendline) == 0)
        {
            printf("\033[1A");
            printf("\033[K"); //清除从光标到行尾的内容
            continue;
        }
        
        send_msg_to_server(sendline);

        string msg = my_name + ":" + sendline;
        message_lists.push_back(msg);

        printf("\033[1A");
        printf("\033[%dD", (strlen(sendline))); //左移strlen(sendline)
        printf("\033[K"); //清除从光标到行尾的内容

        talk_lists_down(true);
        fill_talk_data();
    }
    
}

void fill_talk_data()
{
    printf("\033[11A");

    for (int i = 0; i < TALK_SPACE_SIZE; i++)
    {
        printf("\033[1C");
        int j = 0;
        for (; j < 36; j++)
        {
            printf(" ");
        }
        printf("\033[36D");
        
        int length = MIN(print_talk_lists[i].length(), 36);
        syslog(LOG_ERR, "print_talk_lists[%d]:%s", i, print_talk_lists[i].c_str());
        for (j = 0; j < length; j++)
        {
            printf("%c", print_talk_lists[i][j]);
        }
        
        printf("\n");
    }
    
    printf("\n");
}


void talk_lists_down(bool is_get_msg)
{
    if ((message_lists.size() <= TALK_SPACE_SIZE))
    {
        if (is_get_msg)
        {
            print_talk_lists[g_current_down_pos] = message_lists[g_current_down_pos];
            g_current_down_pos++;
        }
        
        return ;
    }
    else
    {
        
        if (g_current_down_pos >= message_lists.size())
        {
            syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
            return ;
        }

        int i = 0;
        for (; i < TALK_SPACE_SIZE - 1; i++)
        {
            //syslog(LOG_ERR, "print_talk_lists[%d]:%s", i, print_talk_lists[i].c_str());
            print_talk_lists[i] = print_talk_lists[i + 1];
            //syslog(LOG_ERR, "print_talk_lists[%d]:%s", i, print_talk_lists[i].c_str());
        }

        print_talk_lists[i] = message_lists[g_current_down_pos];

        g_current_down_pos++;
        g_current_up_pos++;
       

        syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
    }
}

void talk_lists_up()
{
    if (message_lists.size() < TALK_SPACE_SIZE)
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

        syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
        
        g_current_up_pos--;
        g_current_down_pos--;

        int i = TALK_SPACE_SIZE - 1;
        for (; i > 0; i--)
        {
            syslog(LOG_ERR, "print_talk_lists[i]:%s", print_talk_lists[i].c_str());
            print_talk_lists[i] = print_talk_lists[i - 1];
            syslog(LOG_ERR, "print_talk_lists[i]:%s", print_talk_lists[i].c_str());
        }

        print_talk_lists[i] = message_lists[g_current_up_pos];

        syslog(LOG_ERR, "g_current_up_pos:%d, g_current_down_pos:%d", g_current_up_pos, g_current_down_pos);
    }
}


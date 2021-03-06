#include "tbl_userinfo.h"
#include "tbl_logininfo.h"
#include "tbl_friendinfo.h"
#include "wrapsock_server.h"
#include "notify_client.h"

#define THREAD_COUNT 1
#define  MAXNCLI  100
#define LISTENQ 1024

const int MSG_ID_EVERYONETALK_REGISTER = 1000;
const int MSG_ID_EVERYONETALK_LOGIN = 1001;
const int MSG_ID_EVERYONETALK_FRIEND_LIST = 1002;
const int MSG_ID_EVERYONETALK_CHANGE_USER_STATE = 1003;
const int MSG_ID_EVERYONETALK_SEND_MSG = 1004;
const int MSG_ID_EVERYONETALK_QUIT = 1005;

struct ctrl_send_client
{
    bool is_send_client;
    int notify_type;

    ctrl_send_client()
    {
        is_send_client = false;
        notify_type = 0;
    }
}ctrl_sc;


struct user_state_change
{
    string user_name;
    user_state state;
}us_change;

struct send_massage
{
    string user_name;
    string send_msg;
}s_msg;

pthread_mutex_t    state_mutex;
pthread_cond_t    state_cond;

pthread_t g_thread_tid[THREAD_COUNT];

struct client_socket
{
    int  connfd;
    struct sockaddr_in clisock;
};


void pthread_make_notify(int i);

void deal_msg(int connfd, struct sockaddr_in &clisock);
bool get_msgtpye(char *msg_recv, int &msg_type, string &strMsgData);
int register_info(string strMsgData);
int login(string strMsgData, struct sockaddr_in &clisock);

void set_timer();

void get_friend_list(string strMsg, char *respond);
int change_user_state(string strMsg);
void deal_state_notify(string ip, int port, string user_name, user_state state);
int sendmsg_to_friend(string str_msg);
void deal_send_msg(string ip, int port, string str_msg);
int client_quit(string strMsg);


void *thread_main(void*);

void *thread_notify(void*);

int init_mysql_table()
{
    init_mysql();
    if(connect_mysql())
    {
        syslog(LOG_ERR, "connect_mysql fail.\n");
        return -1;
    }

    init_tbl_user();
    init_tbl_friend();
    return 0;
}


int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;

    socklen_t addrlen, clilen;
    struct sockaddr_in  servaddr;
    client_socket *pc_sock = NULL;
    pthread_t pid;
    
    openlog(argv[0], LOG_PID, 0);

    if(init_mysql_table())
    {
        syslog(LOG_ERR, "init_mysql_table fail.\n");
        return -1;
    }
    
    //设置定时器，定时推送好友状态
    set_timer();

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    pthread_make_notify(10);
    

    for ( ; ; ) {
        clilen = sizeof(struct sockaddr_in);
        pc_sock = (client_socket*)Malloc(sizeof(client_socket));
        pc_sock->connfd = Accept(listenfd, (SA*)&pc_sock->clisock, &clilen);
        Pthread_create(&pid, NULL, &thread_main, (void *)pc_sock);
    }
    
    close_mysql();

    return 0;
    
}

void *thread_main(void *arg)
{
    
    syslog(LOG_ERR, "thread begin %lu starting\n", pthread_self());
    
    client_socket *pc_sock = (client_socket*)arg;
    Pthread_detach(pthread_self());
    deal_msg(pc_sock->connfd, pc_sock->clisock);
    Close(pc_sock->connfd);
    free(pc_sock);
    pc_sock = NULL;

    return NULL;
}

void deal_msg(int connfd, struct sockaddr_in &clisock)
{
    char msg_recv[MAXLINE] = {0};
    int nread = 0;

    while (1)
    {
        if ((nread = Readline(connfd, msg_recv, MAXLINE)) == 0)
        {
            syslog(LOG_ERR, "deal_msg>>Readline socket fail.\n");
            continue;
        }

        syslog(LOG_ERR, "server::deal_msg>>Readline:%s.\n", msg_recv);
        
        int msg_type = 0;
        string strMsgData;
        if (get_msgtpye(msg_recv, msg_type, strMsgData))
        {
            switch (msg_type) 
            {
                case MSG_ID_EVERYONETALK_REGISTER:
                {
                    int ret = register_info(strMsgData);

                    char sendline[10];
                    sprintf(sendline, "%d\n", ret);
                    if(writen(connfd, sendline, strlen(sendline)) != strlen(sendline))
                    {
                       syslog(LOG_ERR, "Readline writen fail.\n");
                    }
                    
                    break;
                }

                case MSG_ID_EVERYONETALK_LOGIN:
                {
                    int ret = login(strMsgData, clisock);

                    char sendline[10];
                    sprintf(sendline, "%d\n", ret);
                    if(writen(connfd, sendline, strlen(sendline)) != strlen(sendline))
                    {
                       syslog(LOG_ERR, "Readline writen fail.\n");
                    }

                    break;
                }

                case MSG_ID_EVERYONETALK_FRIEND_LIST:
                {
                    char respond[MAXLINE];
                    get_friend_list(strMsgData, respond);

                    char sendline[MAXLINE];
                    sprintf(sendline, "%s\n", respond);
                    syslog(LOG_ERR, "sendline %s.\n", respond);
                    
                    if(writen(connfd, sendline, strlen(sendline)) != strlen(sendline))
                    {
                       syslog(LOG_ERR, "Readline writen fail.\n");
                    }
                    
                    break;
                }
                case MSG_ID_EVERYONETALK_CHANGE_USER_STATE:
                {
                    int ret = change_user_state(strMsgData);

                    char sendline[10];
                    sprintf(sendline, "%d\n", ret);
                    syslog(LOG_ERR, "CHANGE_USER_STATE:%s\n", sendline);
                    if(writen(connfd, sendline, strlen(sendline)) != strlen(sendline))
                    {
                       syslog(LOG_ERR, "Readline writen fail.\n");
                    }
                    
                    break;
                }
                case MSG_ID_EVERYONETALK_SEND_MSG:
                {
                    syslog(LOG_ERR, "MSG_ID_EVERYONETALK_SEND_MSG strMsgData:%s\n", strMsgData.c_str());
                    int ret = sendmsg_to_friend(strMsgData);

                    char sendline[10];
                    sprintf(sendline, "%d\n", ret);
                    if(writen(connfd, sendline, strlen(sendline)) != strlen(sendline))
                    {
                       syslog(LOG_ERR, "Readline writen fail.\n");
                    }
                    
                    break;
                }
                case MSG_ID_EVERYONETALK_QUIT:
                {
                    syslog(LOG_ERR, "client quit, so server also quit.\n");
                    // 通知这个用户的好友，这个用户下线了
                    client_quit(strMsgData);
                    syslog(LOG_ERR, "client quit, so server also quit.end\n");
                    return ;
                }
                
                default:
                    break;
            }
        }
    }
    
}


void notify_client()
{
    // 检测当下登陆用户有木有状态变更
    while (1)
    {
        Pthread_mutex_lock(&state_mutex);
        while (!ctrl_sc.is_send_client)
        {
            Pthread_cond_wait(&state_cond, &state_mutex);
        }

        ctrl_sc.is_send_client = false;
        Pthread_mutex_unlock(&state_mutex);

        syslog(LOG_ERR, "ctrl_sc.notify_type:%d", ctrl_sc.notify_type);
        switch (ctrl_sc.notify_type)
        {
            case NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE:
            {
                // 根据用户名， 查出所有好友
                int user_id = get_userid_by_username(us_change.user_name);

                vector<int> userids;
                get_friendids_by_userid(user_id, userids);
                // 根据查出的所有好友id，查出所有在线好友
                vector<tbl_login> t_logins;
                get_logininfos_by_userid(userids, t_logins);
                // 连接所有客户端，推送状态好好友

                syslog(LOG_ERR, "deal_state_notify.t_logins.size:%d", t_logins.size());
                
                for (int i = 0; i < t_logins.size(); i++)
                {
                    syslog(LOG_ERR, "AAAA server notify_client. t_logins[i].user_name:%s", t_logins[i].user_name.c_str());
                    deal_state_notify(t_logins[i].ip, t_logins[i].port, us_change.user_name, us_change.state);
                }
                
                break;
            }
            case NOTIFY_ID_EVERYONETALK_MESSAGE_INFOMATION:
            {
                string str_ip;
                int port = 0;
                get_friend_info(s_msg.user_name, str_ip, port);
                deal_send_msg(str_ip, port, s_msg.send_msg);
                
                break;
            }
            
            default:
                break;
        }
    }
    
}

void deal_state_notify(string ip, int port, string user_name, user_state state)
{
    char request[MAXLINE] = {0};
    sprintf(request, "%d;%s;%d\n", NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE, user_name.c_str(), state);
    syslog(LOG_ERR, "deal_state_notify.request:%s", request);
    deal_notify(ip, port, request);
}

void deal_send_msg(string ip, int port, string str_msg)
{
    char request[MAXLINE] = {0};
    sprintf(request, "%d;%s\n", NOTIFY_ID_EVERYONETALK_MESSAGE_INFOMATION, str_msg.c_str());
    syslog(LOG_ERR, "deal_send_msg.request:%s", request);
    deal_notify(ip, port, request);
}



void set_timer()
{
    /*struct itimerval tick;
    
    signal(SIGALRM, notify_friend_state);
    memset(&tick, 0, sizeof(tick));

     //Timeout to run first time
    tick.it_value.tv_sec = 1;
    tick.it_value.tv_usec = 0;

    //After first, the Interval time for clock
    tick.it_interval.tv_sec = 1;
    tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
            printf("Set timer failed!\n");*/
}

void pthread_make_notify(int i)
{
    Pthread_create(&g_thread_tid[i], NULL, &thread_notify, (void *) i);
}

void *thread_notify(void*)
{
    notify_client();
}




bool get_msgtpye(char *msg_recv, int &msg_type, string &strMsgData)
{
    string strMsg;
    strMsg = msg_recv;

    strMsg = strMsg.substr(0, strMsg.length() - 1);
    
    int pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        msg_type = atoi((strMsg.substr(0, pos)).c_str());
        strMsgData = strMsg.substr(pos + 1);
        return  true;
    }

    return false;
}

void analysis_register_msgdata(string &strMsg, UserInfo &user_info)
{
    
    int pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        //user_info.user_name =  strMsg.substr(0, pos);
        strcpy(user_info.user_name, strMsg.substr(0, pos).c_str());
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        //user_info.passwd =  strMsg.substr(0, pos);
        strcpy(user_info.passwd, strMsg.substr(0, pos).c_str());
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        //user_info.sex = strMsg.substr(0, pos);
        strcpy(user_info.sex, strMsg.substr(0, pos).c_str());
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos) 
    {
        //user_info.birth = strMsg.substr(0, pos);
        strcpy(user_info.birth, strMsg.substr(0, pos).c_str());
       // user_info.hometown = strMsg.substr(pos + 1);
        strcpy(user_info.hometown, strMsg.substr(0, pos).c_str());
    }

}


int register_info(string strMsgData)
{
    syslog(LOG_ERR, "strMsgData:%s\n", strMsgData.c_str());
    UserInfo user_info;
    analysis_register_msgdata(strMsgData, user_info);
    
    insert_user_info(user_info);
    return 0;
}

void analysis_login_msgdata(string &strMsg, LoginInfo &login_info)
{
    int pos = strMsg.find(";");
    if (pos != strMsg.npos) 
    {
        login_info.user_name = strMsg.substr(0, pos);
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos) 
    {
        login_info.passwd = strMsg.substr(0, pos);
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos) 
    {
        login_info.ip = strMsg.substr(0, pos);
        login_info.port = atoi(strMsg.substr(pos + 1).c_str());
    }
}

bool is_login_success(LoginInfo &login_info, vector<tbl_User> &vecTableUSer, int &user_id)
{
    for (int i = 0; i < vecTableUSer.size(); i++)
    {
        if (login_info.user_name == vecTableUSer[i].user_name
            && login_info.passwd == vecTableUSer[i].passwd)
        {
            user_id = vecTableUSer[i].id;
            return true;
        }
    }

    return false;
}

int login(string strMsgData, struct sockaddr_in &clisock)
{
    LoginInfo login_info;
    analysis_login_msgdata(strMsgData, login_info);

    vector<tbl_User> vecTableUser;
    vecTableUser = get_user_nfo();

    int ret = FAIL;
    int user_id;
    if (is_login_success(login_info, vecTableUser, user_id))
    {
        ret = OK;
        
        // 将登录用户放到缓存和数据库
        tbl_login t_login;
        t_login.user_name = login_info.user_name;

        t_login.ip = login_info.ip;
        t_login.port = login_info.port;
        
        time_t now ;
        struct tm *tm_now ;
        time(&now) ;
        tm_now = localtime(&now) ;

        char login_time[MAXLINE] = {0};
        sprintf(login_time, "%d-%d-%d %d:%d:%d", tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec) ;
        t_login.login_time = login_time;

        t_login.state = ONLINE;
        t_login.user_id = user_id;
        
        insert_login_info(t_login);

        Pthread_mutex_lock(&state_mutex);
        
        ctrl_sc.is_send_client = true;
        ctrl_sc.notify_type = NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE;
        us_change.user_name = login_info.user_name;
        us_change.state = t_login.state;
        
        Pthread_cond_signal(&state_cond);
        Pthread_mutex_unlock(&state_mutex);
    }
    return ret;
}

void serialize_string(vector<tbl_User> &t_users, map<int, user_state> &userid_map_states, char *respond)
{
    string strRespond;
    for (int i = 0; i < t_users.size(); i++)
    {
        user_state state = OFFLINE;
        map<int, user_state>::iterator iter = userid_map_states.find(t_users[i].id);
        if (iter != userid_map_states.end())
        {
            state = iter->second;
        }

        char tmpres[MAXLINE] = {0};
        sprintf(tmpres, "%d,%s,%d;", t_users[i].id, t_users[i].user_name.c_str(), state);
        strRespond += tmpres;
    }

    strcpy(respond, strRespond.c_str());
}

void get_friend_list(string strMsg, char *respond)
{
    // 根据用户名，查询出登陆信息
    tbl_login t_login;
    get_logininfo_by_username(strMsg, t_login);
    // 根据用户ID，查询出所有好友ID
    vector<int> userids;
    get_friendids_by_userid(t_login.user_id, userids);
    // 根据好友ID，查询出所有好友信息

    vector<tbl_User> t_users;
    get_friendinfo_by_userid(userids, t_users);

    map<int, user_state> userid_map_states;
    get_friendstates_by_userid(userids, userid_map_states);
    //序列化成字符串
    serialize_string(t_users, userid_map_states, respond);
}

void deserialize_user_state(string &strMsg, string &user_name, user_state &state)
{
    int pos = strMsg.find(";");
    if (pos != string::npos)
    {
        user_name = strMsg.substr(0, pos);
        state = (user_state)atoi(strMsg.substr(pos + 1).c_str());
    }
}

int change_user_state(string strMsg)
{
    // user_name:state
    syslog(LOG_ERR, "AAAA server change_user_state. user_name:%s", strMsg.c_str());
    string user_name;
    user_state state;
    deserialize_user_state(strMsg, user_name, state);

    // 修改緩存和數據庫 login表
    int ret = update_login_state(user_name, state);

    Pthread_mutex_lock(&state_mutex);
        
    ctrl_sc.is_send_client = true;
    ctrl_sc.notify_type = NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE;
    us_change.user_name = user_name;
    us_change.state = state;
    
    Pthread_cond_signal(&state_cond);
    Pthread_mutex_unlock(&state_mutex);

    return ret;
}

int client_quit(string strMsg)
{
    if (strMsg == "none")
    {
        return 0;
    }
    
    int ret = update_quit_state(strMsg, OFFLINE);

    Pthread_mutex_lock(&state_mutex);
        
    ctrl_sc.is_send_client = true;
    ctrl_sc.notify_type = NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE;
    us_change.user_name = strMsg;
    us_change.state = OFFLINE;
    
    Pthread_cond_signal(&state_cond);
    Pthread_mutex_unlock(&state_mutex);

    return ret;
}


void deserialize_user_msg(string strMsg, string &user_name, string &user_msg)
{
    int pos = strMsg.find(";");
    if (pos != string::npos)
    {
        user_name = strMsg.substr(0, pos);
        user_msg = strMsg.substr(pos + 1);
    }
}


int sendmsg_to_friend(string str_msg)
{
    string user_name;
    string user_msg;
    deserialize_user_msg(str_msg, user_name, user_msg);

    Pthread_mutex_lock(&state_mutex);
        
    ctrl_sc.is_send_client = true;
    ctrl_sc.notify_type = NOTIFY_ID_EVERYONETALK_MESSAGE_INFOMATION;
    s_msg.user_name = user_name;
    s_msg.send_msg = user_msg;
    
    Pthread_cond_signal(&state_cond);
    Pthread_mutex_unlock(&state_mutex);

    return 0;
}

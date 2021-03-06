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
const int MSG_ID_EVERYONETALK_QUIT = 4999;


struct user_state_change
{
    bool is_change_user_state;
    string user_name;
}us_change;

pthread_mutex_t    state_mutex;
pthread_cond_t    state_cond;


struct client_conn
{
    int  connfd;
    struct sockaddr_in *clisock;
};

client_conn g_cliconn[MAXNCLI];
int iget, iput;
pthread_mutex_t    clifd_mutex;
pthread_cond_t    clifd_cond;

pthread_t g_thread_tid[THREAD_COUNT];

void pthread_make_main(int i);
void pthread_make_notify(int i);

void deal_msg(int connfd, struct sockaddr_in *clisock);
bool get_msgtpye(char *msg_recv, int &msg_type, string &strMsgData);
int register_info(string strMsgData);
int login(string strMsgData, struct sockaddr_in *clisock);

void set_timer();

void get_friend_list(string strMsg, char *respond);
void change_user_state(string strMsg);
void deal_state_notify(string ip, user_state state);

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

    return 0;
}


int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;

    socklen_t addrlen, clilen;
    struct sockaddr_in  servaddr;
    struct sockaddr_in  *cliaddr;
    
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

    

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_make_main(i);
    }

    pthread_make_notify(10);
    

    for ( ; ; ) {
        cliaddr = (struct sockaddr_in*)Malloc(addrlen);
        clilen = addrlen;
        connfd = Accept(listenfd, (SA*)cliaddr, &clilen);

        Pthread_mutex_lock(&clifd_mutex);
        g_cliconn[iput].connfd = connfd;
        g_cliconn[iput].clisock = cliaddr;
        if (++iput == MAXNCLI)
            iput = 0;
        if (iput == iget)
            syslog(LOG_ERR, "iput = iget = %d", iput);
        Pthread_cond_signal(&clifd_cond);
        Pthread_mutex_unlock(&clifd_mutex);
    }

    close_mysql();

    return 0;
    
}

void notify_client()
{
    // 检测当下登陆用户有木有状态变更
    us_change.is_change_user_state = false;
    while (1)
    {
        Pthread_mutex_lock(&state_mutex);
        while (!us_change.is_change_user_state)
        {
            
            Pthread_cond_wait(&state_cond, &state_mutex);
        }

        us_change.is_change_user_state = false;
        Pthread_mutex_unlock(&state_mutex);

        // 通知所有好友

        // 根据用户名， 查出所有好友
        int user_id = get_userid_by_username(us_change.user_name);

        vector<int> userids;
        get_friendids_by_userid(user_id, userids);
        // 根据查出的所有好友id，查出所有在线好友
        vector<tbl_login> t_logins;
        get_logininfos_by_userid(userids, t_logins);
        // 连接所有客户端，推送状态好好友

        for (int i = 0; i < t_logins.size(); i++)
        {
            deal_state_notify(t_logins[i].ip, t_logins[i].state);
        }
        
        
    }
    
}

void deal_state_notify(string ip, user_state state)
{
    char request[MAXLINE] = {0};
    sprintf(request, "%d", state);
    deal_notify(ip, request);
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


void pthread_make_main(int i)
{
    Pthread_create(&g_thread_tid[i], NULL, &thread_main, (void *) i);
}

void pthread_make_notify(int i)
{
    Pthread_create(&g_thread_tid[i], NULL, &thread_notify, (void *) i);
}

void *thread_notify(void*)
{
    notify_client();
}


void *thread_main(void *arg)
{
    syslog(LOG_ERR, "thread begin %lu starting\n", pthread_self());
    int connfd;
    struct sockaddr_in *clisock;
    
    int *p = (int*)arg;
    //syslog(LOG_ERR, "thread %d starting\n", *p);
    for ( ; ; ) 
    {
        Pthread_mutex_lock(&clifd_mutex);
        while (iget == iput) 
        {
            Pthread_cond_wait(&clifd_cond, &clifd_mutex);
            syslog(LOG_ERR, "Pthread_cond_wait end.iget:%d, iput:%d\n", iget, iput);
        }

        syslog(LOG_ERR, "connfd = clifd[iget];\n");
        connfd = g_cliconn[iget].connfd;
        clisock = g_cliconn[iget].clisock;
        if (++iget == MAXNCLI) {
            iget = 0;
        }
        Pthread_mutex_unlock(&clifd_mutex);

        deal_msg(connfd, clisock);
        Close(connfd);
    }
}

void deal_msg(int connfd, struct sockaddr_in *clisock)
{
    char msg_recv[MAXLINE] = {0};
    int nread = 0;

    while (1)
    {
        if ((nread = Readline(connfd, msg_recv, MAXLINE)) == 0)
        {
            syslog(LOG_ERR, "Readline socket fail.\n");
            continue;
        }

        syslog(LOG_ERR, "Readline:%s.\n", msg_recv);
        
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
                    sprintf(sendline, "%d", ret);
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
                    sprintf(sendline, "%d", ret);
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
                    break;
                }

                case MSG_ID_EVERYONETALK_QUIT:
                {
                    syslog(LOG_ERR, "client quit, so server also quit.\n");
                    // 通知这个用户的好友，这个用户下线了
                    return ;
                }
                
                default:
                    break;
            }
        }
    }
    
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
        user_info.user_name =  strMsg.substr(0, pos);
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        user_info.passwd =  strMsg.substr(0, pos);
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        user_info.sex = strMsg.substr(0, pos);
        strMsg = strMsg.substr(pos + 1);
    }

    pos = strMsg.find(";");
    if (pos != strMsg.npos) 
    {
        user_info.birth = strMsg.substr(0, pos);
        user_info.hometown = strMsg.substr(pos + 1);
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
        login_info.passwd = strMsg.substr(pos + 1);
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

int login(string strMsgData, struct sockaddr_in *clisock)
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

        char ip_address[MAXLINE] = {0};
        Inet_ntop(AF_INET, &clisock->sin_addr, ip_address, MAXLINE);
        t_login.ip = ip_address;

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
        
        us_change.is_change_user_state = true;
        us_change.user_name = login_info.user_name;
        
        Pthread_cond_signal(&state_cond);
        Pthread_mutex_unlock(&state_mutex);
    }
    return ret;
}

void serialize_string(vector<tbl_User> &t_users, map<int, user_state> &userid_map_states, char *respond)
{
    for (int i = 0; i < t_users.size(); i++)
    {
        string state;
        map<int, user_state>::iterator iter = userid_map_states.find(t_users[i].id);
        if (iter != userid_map_states.end())
        {
            state = iter->second;
        }

        char tmpres[MAXLINE] = {0};
        sprintf(tmpres, "%d,%s,%d;", t_users[i].id, t_users[i].user_name.c_str(), state);
        respond = strcat(tmpres, respond);
    }
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

void change_user_state(string strMsg)
{
    // user_name:state
    string user_name;
    user_state state;
    deserialize_user_state(strMsg, user_name, state);

    // 修改緩存和數據庫 login表
    update_login_state(user_name, state);

    Pthread_mutex_lock(&state_mutex);
        
    us_change.is_change_user_state = true;
    us_change.user_name = user_name;
    
    Pthread_cond_signal(&state_cond);
    Pthread_mutex_unlock(&state_mutex);
}

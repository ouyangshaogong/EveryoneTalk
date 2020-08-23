#include <stdio.h>	
#include "client_gui.h"
#include "deal_friend_data.h"
#include "deal_talk_data.h"
#include "read_config.h"

int g_interface_friend = 0;
int g_interface_talk = 0;

int g_sockfd = 0;

int    clifd[MAXNCLI], iget, iput;
pthread_mutex_t    clifd_mutex;
pthread_cond_t    clifd_cond;

pthread_t g_thread_tid[THREAD_COUNT];

vector<friend_list> *pf_lists;
bool g_friend_state = false;

const int FRIEND_SPACE_SIZE = 10;
extern friend_list print_f_lists[10];


char pname[MAXLINE];

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)

string s_ip;
int s_port = 0;
string c_ip;
int c_port = 0;


//分配内存的大小
#define     SIZE    10
//定义按键们的宏
#define     ESC     "\033"
#define     UP      "UP"
#define     DOWN    "DOWN"
#define     LEFT    "\033[D"
#define     RIGHT   "\033[C"

#define     MODIFY_STATE     "modify"
#define     QUERY_INFO       "query"
#define     SHOW_FRIEND      "show"
#define     SWITCH_ACCOUNT   "switch"
#define     QUIT             "quit"
#define     HELP             "help"

#define    SEND_MSG          "send"
#define    QUERY_FRIEND      "query"
#define    SHOW_MSG          "show"

int set_disp_mode(int fd,int option)
{
   int err;
   struct termios term;
   if(tcgetattr(fd,&term)==-1){
     perror("Cannot get the attribution of the terminal");
     return 1;
   }
   if(option)
        term.c_lflag|=ECHOFLAGS;
   else
        term.c_lflag &=~ECHOFLAGS;
   err=tcsetattr(fd,TCSAFLUSH,&term);
   if(err==-1 && err==EINTR){
        perror("Cannot set the attribution of the terminal");
        return 1;
   }
   return 0;
}

void help_cmd(char *c_cmd)
{
    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容
    
    printf("Usage:\n");

    printf("%s", c_cmd);

    getchar();

    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容
    printf("\033[1A");
    printf("\033[K"); //清除从光标到行尾的内容
}

void deal_main_cmd(bool &is_quit)
{
    char c_cmd[SIZE] = {0};
    get_main_cmd(c_cmd);
    if (!strcmp(c_cmd, "register"))
    {
        clear_string_surface(9);
        print_register();
    }
    else if (!strcmp(c_cmd, "login"))
    {
        printf("\033[1A");
        printf("\033[K"); //清除从光标到行尾的内容
        deal_login_data(is_quit);
    }
    else if (!strcmp(c_cmd, "help"))
    {
        char c_cmd[] = ":help <help|register|login|quit>";
        help_cmd(c_cmd);
    }
    else if (!strcmp(c_cmd, "quit"))
    {
        is_quit = true;
        string user_name = "none";
        quit_from_server(user_name);
    }
    else
    {
       deal_error_cmd();
    }
}

void get_main_cmd(char *c_cmd)
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
            strcpy(c_cmd, t_cmd);
            
            break;
        }
        else 
        {
             tcsetattr(0, TCSANOW, &old_opt); //恢复
        }
    }
}

static void
sig_int(int signo)
{
    syslog(LOG_ERR, "sig_int:%d");
}


int main(int argc, char *argv[])
{
    read_client_data(c_ip, c_port, s_ip, s_port);
    syslog(LOG_ERR, "c_ip:%s, c_port:%d", c_ip.c_str(), c_port);
    syslog(LOG_ERR, "s_ip:%s, s_port:%d", s_ip.c_str(), s_port);
    
    strcpy(pname, argv[0]);
    
    int use_type = 0;

    openlog(argv[0], LOG_PID, 0);

    pthread_make(0);

    if (signal(SIGINT, sig_int) == SIG_ERR)
        syslog(LOG_ERR, "signal(SIGINT) error");

    if (client_connect(s_ip.c_str(), s_port))
    {
        restore_client_data(c_port);
        printf("client_connect fail.\n");
        return -1;
    }
    
    while (1)
    {
        print_login();
        bool is_quit = false;
        deal_main_cmd(is_quit);

        if (is_quit)
        {
            break;
        }

        clear_string_surface(9);
    }
    
    Close(g_sockfd);

    restore_client_data(c_port);
    return 0;
}

void print_register()
{
    printf("+-----------+-----------------+\n");
    printf("| UserInfo  | Value           |\n");
    printf("+-----------+-----------------+\n");
    printf("| username: |                 |\n");
    printf("| passwd:   |                 |\n");
    printf("| repasswd: |                 |\n");
    printf("| sex:      |                 |\n");
    printf("| birth:    |                 |\n");
    printf("| hometown: |                 |\n");
    printf("+-----------+-----------------+\n");

    UserInfo user_info;
    char repasswd[MAXLINE];
    
    printf("\033[7A");  //上移8位
    printf("\033[15C"); //右移15位
    scanf("%s", user_info.user_name);  // 输入username

    printf("\033[15C"); //右移15位
    scanf("%s", user_info.passwd); 
    
    printf("\033[15C"); //右移15位
    scanf("%s", repasswd);
    
    printf("\033[15C"); //右移15位
    scanf("%s", user_info.sex);

    printf("\033[15C"); //右移15位
    scanf("%s", user_info.birth);

    printf("\033[15C"); //右移15位
    scanf("%s", user_info.hometown);

    int ret = FAIL;
    if (strcmp(user_info.passwd, repasswd) == 0) {
        ret = register_to_server(user_info); //注册到服务器
    }

    printf("\n");
    if (ret == 0) {
        printf("register successful.\n");
    }
    else {
        printf("register fail.\n");
    }

    setbuf(stdin,NULL); //清空输入缓冲区
    getchar();   //暂停

    // 清空注册界面
    clear_string_surface(12);

    return ;
}

void get_login_data(string &l_user)
{
    char l_data[10];
    while (1)
    {   
        struct termios old_opt, opt;
    
        tcgetattr(0, &old_opt);
        opt = old_opt;

        opt.c_lflag &= ~ICANON;
        
        
        int i = 0;
        char ch;
        syslog(LOG_ERR, "l_user:%s", l_user.c_str());

        //tcsetattr(0, TCSANOW, &opt); //设置
        while ((ch = getchar()) != '\n')
        {
            l_data[i] = ch;
            i++;
            syslog(LOG_ERR, "i:%d", i);
            if (i == 11)
            {
                printf("\n");
                printf("\033[11D"); //右移14位
                break;
            }
        }
        //tcsetattr(0, TCSANOW, &old_opt); //设置
        
        l_data[i] = '\0';
        if (strlen(l_data) != 0)
        {
            syslog(LOG_ERR, "strlen(l_data):%d", strlen(l_data));
            break;
        }
        else
        {
            printf("\033[1A");  //上移1位
            printf("\033[14C"); //右移14位
        }
    }
    
    l_user = l_data;
}
void deal_login_data(bool &is_quit)
{
    LoginInfo login_info;

    printf("\033[6A");  //上移4位
    printf("\033[14C"); //右移14位
    get_login_data(login_info.user_name);
    
    printf("\033[1B");  //下移1位
    printf("\033[14C"); //右移14位
    get_login_data(login_info.passwd);
    printf("\n\n\n");  //下移3位

    login_info.ip = c_ip;
    login_info.port = c_port;
    int ret = login(login_info);
    
    if (ret == 0) 
    {
        printf("login successful.\n");
        setbuf(stdin,NULL); //清空输入缓冲区
        getchar();   //暂停

        // 清空界面
        clear_string_surface(11);
        deal_friend_list(login_info.user_name, is_quit);
    }
    else
    {
        printf("login fail.\n");
        setbuf(stdin,NULL); //清空输入缓冲区
        getchar();   //暂停

        // 清空注册界面
        clear_string_surface(11);
        return ;
    }
}

void print_login()
{
    printf("+----+------+-----------------+\n");
    printf("|    |     EveryoneTalk       |\n");
    printf("+----+------+-----------------+\n");
    printf("| UserName: |                 |\n");
    printf("+-----------+-----------------+\n");
    printf("| Passwd:   |                 |\n");
    printf("+-----------+---+-------------+\n");
    printf("|     login     |  register   |\n");
    printf("+---------------+-------------+\n");
}

void pthread_make(int i)
{
    void *thread_main(void*);

    Pthread_create(&g_thread_tid[i], NULL, &thread_main, (void *) i);
}

void *thread_main(void *arg)
{
    int listenfd;
    int connfd;
    
    socklen_t addrlen, clilen;
    struct sockaddr_in  servaddr;
    struct sockaddr_in  *cliaddr;
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(c_port);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    cliaddr = (struct sockaddr_in*)Malloc(addrlen);

    for ( ; ; ) {
        clilen = addrlen;
        connfd = Accept(listenfd, (SA*)cliaddr, &clilen);

        deal_noyify(connfd);

        Close(connfd);
        //free(cliaddr);
    }
}

bool get_notify_type(char *notify_recv, int &notify_type, string &strNotifyData)
{
    string strMsg;
    strMsg = notify_recv;

    strMsg = strMsg.substr(0, strMsg.length() - 1);
    
    int pos = strMsg.find(";");
    if (pos != strMsg.npos)
    {
        notify_type = atoi((strMsg.substr(0, pos)).c_str());
        strNotifyData = strMsg.substr(pos + 1);
        
        return  true;
    }

    return false;
}



void deal_noyify(int connfd)
{
    char notify_recv[MAXLINE] = {0};
    int nread = 0;

    if ((nread = Readline(connfd, notify_recv, MAXLINE)) == 0)
    {
        syslog(LOG_ERR, "deal_noyify Readline socket fail.\n");
        return ;
    }

    syslog(LOG_ERR, "AAAA client deal_noyify. notify_recv:%s", notify_recv);
    
    int notify_type;
    string strNotifyData;
    if (get_notify_type(notify_recv, notify_type, strNotifyData))
    {
        syslog(LOG_ERR, "notify_type:%d, strNotifyData:%s\n", notify_type, strNotifyData.c_str());
        switch (notify_type)
        {
            case NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE:
            {
                string user_name;
                user_state state;
                friend_online_state_deserialize(strNotifyData, user_name, state);
                syslog(LOG_ERR, "user_name:%s, state:%d\n", user_name.c_str(), state);
                modify_friend_state(user_name, state);

                break;
            }
            case NOTIFY_ID_EVERYONETALK_MESSAGE_INFOMATION:
            {
                syslog(LOG_ERR, "strNotifyData:%s\n", strNotifyData.c_str());
                recvmsg_from_server(strNotifyData);
                break;
            }
            
            default:
                break;
        }
    }

    char pRet[30] = "hahahahahahahaha\n";
    if(writen(connfd, pRet, strlen(pRet)) != strlen(pRet))
    {
        syslog(LOG_ERR, "write to socket fail.\n");
        return ;
    }
}

void get_friend_cmd(char *c_cmd)
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
        if( ch == 27 )
        {
            getchar();
            if( (ch = getchar()) == 65 || ch == 66 )
            {
                if(ch == 65) //上键
                {
                    strcpy(c_cmd, "UP");
                }
                else if(ch == 66) //下键
                {
                    strcpy(c_cmd, "DOWN");
                }

                tcsetattr(0, TCSANOW, &old_opt); //恢复
                break;
            }
        }
        else if (ch == 58)
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
            strcpy(c_cmd, t_cmd);
            
            break;
        }
        else 
        {
             tcsetattr(0, TCSANOW, &old_opt); //恢复
        }
    }
}


void deal_friend_list_cmd(vector<friend_list> &f_lists, string my_name, bool &is_quit)
{
    char c_key[SIZE] = {0};

    get_friend_cmd(c_key);

    if (!strcmp(c_key, UP))
    {
        friend_lists_up(f_lists);
    }
    else if(!strcmp(c_key, DOWN))
    {
        friend_lists_down(f_lists);
    }
    else if(!strcmp(c_key, MODIFY_STATE))
    {
        friend_lists_modify(my_name);
    }
    else if(!strcmp(c_key, QUERY_INFO))
    {
        friend_lists_query();
    }
    else if(!strcmp(c_key, SHOW_FRIEND))
    {
        friend_lists_show();
    }
    else if(!strcmp(c_key, SWITCH_ACCOUNT))
    {
        friend_lists_switch();
    }
    else if(!strcmp(c_key, QUIT))
    {
        friend_lists_quit(my_name, is_quit);
    }
    else if(!strcmp(c_key, HELP))
    {
        char c_content[MAXLINE] = ":help <help|up|down|modify|query|show|switch|quit>\n";
        strcat(c_content, "user_state:<ONLINE|OFFLINE|LEAVE|BUSY|NOT_DISTURB|ONLINE>");
        help_cmd(c_content);

        printf("\033[1A");
        printf("\033[K"); //清除从光标到行尾的内容
    }
    else if ((strlen(c_key) != 0) && strspn(c_key, "123456789") == strlen(c_key))
    {
        int user_id = atoi(c_key);
        string f_name;
        if (is_user_exist(f_lists, user_id, f_name))
        {
            // 清空注册界面
            clear_string_surface(17);
            g_interface_friend = 0;
            g_interface_talk = 1;
            deal_talk_data(my_name, f_name);
            g_interface_talk = 0;
            g_interface_friend = 1;
            print_friend_list();
        }
    }
    else
    {
        deal_error_cmd();
    }
}

void deal_friend_data(vector<friend_list> &f_lists, string my_name, bool &is_quit)
{
    init_friend_lists(f_lists);
    init_user_state();
    while(1)
    {
        fill_friend_data();
        deal_friend_list_cmd(f_lists, my_name, is_quit);
        if (is_quit)
        {
            break;
        }
    }
}


void deal_friend_list(string user_name, bool &is_quit)
{
    print_friend_list();
    
    vector<friend_list> f_lists;
    pf_lists = &f_lists;
    
    g_interface_friend = 1;
    if (get_friend_list(user_name, f_lists))
    {
        printf("get_friend_list fail.\n");
        return;
    }

    deal_friend_data(f_lists, user_name, is_quit);
   
    g_interface_friend = 0;
    // 清空注册界面
    clear_string_surface(16);
}


void print_friend_list()
{
    printf("+--------+--------------+---------+\n");
    printf("|         EveryoneTalk            |\n");
    printf("+--------+--------------+---------|\n");
    printf("|  ID    |  UserName    |  State  |\n");
    printf("+--------+--------------+---------|\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("|        |              |         |\n");
    printf("+--------+--------------+---------+\n");
}

void get_talk_cmd(char *c_key)
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
        if( ch == 27 )
        {
            getchar();
            if( (ch = getchar()) == 65 || ch == 66 )
            {
                if(ch == 65) //上键
                {
                    strcpy(c_key, "UP");
                }
                else if(ch == 66) //下键
                {
                    strcpy(c_key, "DOWN");
                }

                tcsetattr(0, TCSANOW, &old_opt); //恢复
                break;
            }
        }
        else if (ch == 58)
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

void deal_talk_cmd(string my_name, string f_name, bool &is_quit)
{
    char c_key[SIZE] = {0};
    get_talk_cmd(c_key);

    if (!strcmp(c_key, SEND_MSG))
    {
        printf("\033[1A");
        printf("\033[%dD", strlen(c_key) + 1);
        printf("\033[K"); //清除从光标到行尾的内容
        
        get_send_msg(my_name, f_name);
    }
    else if(!strcmp(c_key, QUERY_FRIEND))
    {
        
    }
    else if(!strcmp(c_key, SHOW_MSG))
    {
        
    }
    else if(!strcmp(c_key, QUIT))
    {
        printf("\033[1A");
        printf("\033[%dD", strlen(c_key) + 1);
        printf("\033[K"); //清除从光标到行尾的内容
        
        is_quit = true;
    }
    else if (!strcmp(c_key, LEFT))
    {

    }
    else if (!strcmp(c_key, RIGHT))
    {

    }
    else if (!strcmp(c_key, HELP))
    {
        char c_content[MAXLINE] = ":help <help|up|down|send|query|show|quit>";
        help_cmd(c_content);
    }
    else if(!strcmp(c_key, UP))
    {
        talk_lists_up();
    }
    else if (!strcmp(c_key, DOWN))
    {
        talk_lists_down(false);
    }
    else
    {
        deal_error_cmd();
    }
}

void deal_error_cmd()
{
    printf("\033[1A");
    printf("input error. please input help.");
    setbuf(stdin,NULL); //清空输入缓冲区
    getchar();

    printf("\033[1A");
    printf("\033[32D");
    printf("\033[K"); //清除从光标到行尾的内容
}

void deal_talk_data(string my_name, string f_name)
{
    print_talk();

    init_talk_lists(f_name);
    char c_key[SIZE] = {0};
    while(1)
    {
        fill_talk_data();

        bool is_quit = false;
        deal_talk_cmd(my_name, f_name, is_quit);

        if (is_quit)
        {
            clear_string_surface(14);
            break;
        }
    }
    
}

void print_talk()
{
    printf("+------+------------+----------------+\n");
    printf("|                                    |\n");
    printf("+------------------------------------|\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("|                                    |\n");
    printf("+------------------------------------+\n");
}


void clear_string_surface(int move_up)
{
    for(int i = 0; i < move_up; i++)
    {
        printf("\033[1A"); //上移1行
        printf("\033[K"); //清除从光标到行尾的内容
    }
}


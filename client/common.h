#ifndef __COMMON_CLIENT__
#define __COMMON_CLIENT__


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /* inet(3) functions */
#include <errno.h>
#include <fcntl.h>  /* for nonblocking */
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <sys/uio.h>  /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>  /* for Unix domain sockets */
#include <sys/types.h>    /* basic system data types */
#include <sys/socket.h>    /* basic socket definitions */
#include <syslog.h>

using namespace std;

#define OK 0

#define FAIL 65535

#define NOTIFY_ID_EVERYONETALK_FRIEND_ONLINE_STATE 5000
#define NOTIFY_ID_EVERYONETALK_MESSAGE_INFOMATION  5001

const int MSG_ID_EVERYONETALK_REGISTER = 1000;
const int MSG_ID_EVERYONETALK_LOGIN = 1001;
const int MSG_ID_EVERYONETALK_FRIEND_LIST = 1002;
const int MSG_ID_EVERYONETALK_CHANGE_USER_STATE = 1003;
const int MSG_ID_EVERYONETALK_SEND_MSG = 1004;
const int MSG_ID_EVERYONETALK_QUIT = 1005;





#define SERV_PORT 15028

#define SERV_PORT_CLIENT 15029


#define SA  struct sockaddr

#define MAXLINE 1024

// 用户信息
struct UserInfo
{
    char user_name[MAXLINE];
    char passwd[MAXLINE];
    char sex[MAXLINE];
    char birth[MAXLINE];
    char hometown[MAXLINE];
};

enum user_state
{
    ONLINE,
    OFFLINE,
    LEAVE,
    BUSY,
    NOT_DISTURB,
    STEALTH
};

struct LoginInfo
{
    string user_name;
    string passwd;
    string ip;
    int port;
};

struct friend_list
{
    int id;
    string user_name;
    user_state state;
};

struct online_state
{
    int id;
    user_state state;
};





void    Pthread_create(pthread_t *, const pthread_attr_t *, void * (*)(void *), void *);
void    Pthread_join(pthread_t, void **);
void    Pthread_detach(pthread_t);
void    Pthread_kill(pthread_t, int);

void    Pthread_mutexattr_init(pthread_mutexattr_t *);
void    Pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void    Pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void    Pthread_mutex_lock(pthread_mutex_t *);
void    Pthread_mutex_unlock(pthread_mutex_t *);

void    Pthread_cond_broadcast(pthread_cond_t *);
void    Pthread_cond_signal(pthread_cond_t *);
void    Pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
void    Pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
                               const struct timespec *);

void    Pthread_key_create(pthread_key_t *, void (*)(void *));
void    Pthread_setspecific(pthread_key_t, const void *);
void    Pthread_once(pthread_once_t *, void (*)(void));

int Socket(int family, int type, int protocol);
void Listen(int fd, int backlog);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void *Malloc(size_t size);
void Close(int fd);
ssize_t Readline(int fd, void *ptr, size_t maxlen);
ssize_t writen(int fd, const void *vptr, size_t n);

#endif

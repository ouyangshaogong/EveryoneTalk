#ifndef __WRAP_SOCK__
#define __WRAP_SOCK__


#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include "common.h"

int Socket(int family, int type, int protocol);
void Inet_pton(int family, const char *strptr, void *addrptr);
int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t Readline(int fd, void *ptr, size_t maxlen);
void Close(int fd);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Listen(int fd, int backlog);
void *Malloc(size_t size);
void friend_online_state_deserialize(string strNotifyData, vector<online_state> &online_states);
int get_friend_list(string user_name, vector<friend_list> &f_lists);
const char *Inet_ntop(int family, struct sockaddr_in *p_sin, char *c_ip, char *c_port, size_t len);
#endif
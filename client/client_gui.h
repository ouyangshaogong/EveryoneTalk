#ifndef __CLIENT_GUI__
#define __CLIENT_GUI__

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include "common.h"
#include "connect_server.h"
#include "everyone_talk.h"

#define REGISTER 1
#define LOGIN 2
#define QUIT_P 3

#define THREAD_COUNT 1
#define  MAXNCLI  100
#define LISTENQ 1024

void pthread_make(int i);

void print_main();
void print_register();
void print_login();
void print_talk();
void deal_noyify(int connfd);
void print_friend_list();
void deal_friend_list(string user_name, bool &is_quit);
void deal_talk_cmd(string my_name, string f_name, bool &is_quit);
void deal_friend_data(vector<friend_list> &f_lists, string my_name, bool &is_quit);
void clear_string_surface(int move_up);

void deal_talk_data(string my_name, string f_name);

void deal_main_cmd(bool &is_quit);
void get_main_cmd(char *c_cmd);
void deal_error_cmd();
#endif
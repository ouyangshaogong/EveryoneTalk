#ifndef __CONNECT_SERVER__
#define __CONNECT_SERVER__

#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include "common.h"
#include "wrapsock.h"

using namespace std;

int client_connect(string ip_address);
int deal_msg(char *pMsg, char *pRet, int nlength);

#endif
#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include "common.h"
#include "wrapsock_server.h"

using namespace std;


int client_connect(string ip_address, int port);
int deal_notify(string ip, int port, char *pNotify);



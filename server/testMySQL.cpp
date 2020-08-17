#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

extern void init_mysql();
extern int connect_mysql();
extern void select_mysql(string strSQL);
extern int close_mysql();

int main(int argc, char *argv[])
{
    string strSQL("select * from user_info");
    init_mysql();
    connect_mysql();
    select_mysql(strSQL);
    close_mysql();
    return 0;
}
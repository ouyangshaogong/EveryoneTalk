#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<mysql/mysql.h>
#include <string>
#include <vector>
#include <map>
#include "common.h"

using namespace std;



void init_mysql();
int connect_mysql();
int select_mysql(string strSQL, vector<map<string, string> > &tables_data);
void insert_mysql(string strSQL);
int update_mysql(string strSQL);
void delete_mysql(string strSQL);
void close_mysql();
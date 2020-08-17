#include "execute_sql.h"

static MYSQL g_connection;

void init_mysql()
{
    mysql_init(&g_connection);
}

int select_mysql(string strSQL, vector<map<string, string> > &tables_data)
{
    int res = mysql_query(&g_connection, strSQL.c_str());//查询
    if(!res)
    {
        MYSQL_RES *result = mysql_store_result(&g_connection);
         
        int num_fields = mysql_num_fields(result);
        MYSQL_FIELD *fields = mysql_fetch_fields(result);
        
        MYSQL_ROW row;
        while (NULL != (row = mysql_fetch_row(result)))
        {
            int i = 0;
            map<string, string> table_data;

            for (int i = 0; i < num_fields; i++)
            {
                table_data.insert(map<string, string>::value_type(fields[i].name, row[i]));
            }

            tables_data.push_back(table_data);
        }
        

        mysql_free_result(result);

        return OK;
    }

    return FAIL;
}


int connect_mysql()
{
    if(!mysql_real_connect(&g_connection,"192.168.2.107","root","qazwsx","everyone_talk",3306,NULL,0))
    {
        printf("connect_mysql error:%s", mysql_error(&g_connection));
        return FAIL;
    }

    vector<map<string, string> > tables_data;
    select_mysql("select * from user_info", tables_data);

    return OK;
}



void insert_mysql(string strSQL)
{
    int res = mysql_query(&g_connection, strSQL.c_str());
    if(!res)
    {
          printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(&g_connection));
    }
    else
    {
          printf("Insert error %d: %s\n",mysql_errno(&g_connection),mysql_error(&g_connection));
    }
}

int update_mysql(string strSQL)
{
    int res = mysql_query(&g_connection, strSQL.c_str());
    if(!res)
    {
          printf("update %lu rows\n",(unsigned long)mysql_affected_rows(&g_connection));
          return OK;
    }
    else
    {
          printf("update error %d: %s\n",mysql_errno(&g_connection),mysql_error(&g_connection));
          return FAIL;
    }

    
}

void delete_mysql(string strSQL)
{
    int res = mysql_query(&g_connection, strSQL.c_str());
    if(!res)
    {
          printf("delete rows successful.\n");
    }
    else
    {
          printf("update error %d: %s\n",mysql_errno(&g_connection),mysql_error(&g_connection));
    }
}


void close_mysql()
{
    mysql_close(&g_connection);
}

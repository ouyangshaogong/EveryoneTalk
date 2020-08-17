#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>   
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "common.h"

using namespace std;

void IniReadValue(char* section, char* key, char* val, const char* file);
int readStringValue(const char* section, char* key, char* val, const char* file);
int readIntValue(const char* section, char* key, const char* file);
void IniWriteValue(const char* section, char* key, char* val, const char* file);
int writeStringVlaue(const char* section, char* key, char* val, const char* file);
int writeIntValue(const char* section, char* key, int val, const char* file);
void read_client_data(int &val_instance, int &val_port);
void restore_client_data(int val_instance, int val_port);





#include "read_config.h"

#define READ_STR_ERR -1
#define READ_STR_OK 0


#define SECTION_MAX_LEN  256
#define STRVALUE_MAX_LEN  256
#define LINE_CONTENT_MAX_LEN 256
//read value from .ini
void IniReadValue(char* section, char* key, char* val, const char* file)
{
    FILE* fp;
    int i = 0;
    int lineContentLen = 0;
    int position = 0;
    char lineContent[LINE_CONTENT_MAX_LEN];
    bool bFoundSection = false;
    bool bFoundKey = false;
    fp = fopen(file, "r");
    if(fp == NULL)
    {
        syslog(LOG_ERR, "%s: Opent file %s failed.\n", __FILE__, file);
        return;
    }
    while(feof(fp) == 0)
    {
        memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
        fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
        if((lineContent[0] == ';') || (lineContent[0] == '\0') || (lineContent[0] == '\r') || (lineContent[0] == '\n'))
        {
            continue;
        }

        //check section
        if(strncmp(lineContent, section, strlen(section)) == 0)
        {
            bFoundSection = true;
            //printf("Found section = %s\n", lineContent);
            while(feof(fp) == 0)
            {
                memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
                fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
                //check key
                if(strncmp(lineContent, key, strlen(key)) == 0)
                {
                    bFoundKey = true;
                    lineContentLen = strlen(lineContent);
                    //find value
                    for(i = strlen(key); i < lineContentLen; i++)
                    {
                        if(lineContent[i] == '=')
                        {
                            position = i + 1;
                            break;
                        }
                    }
                    if(i >= lineContentLen) break;
                    strncpy(val, lineContent + position, strlen(lineContent + position));
                    lineContentLen = strlen(val);
                    for(i = 0; i < lineContentLen; i++)
                    {
                        if((lineContent[i] == '\0') || (lineContent[i] == '\r') || (lineContent[i] == '\n'))
                        {
                            val[i] = '\0';
                            break;
                        }
                    }  
                }
                else if(lineContent[0] == '[') 
                {
                    break;
                }
            }
            break;
        }
    }
    if(!bFoundSection){syslog(LOG_ERR, "No section = %s\n", section);}
    else if(!bFoundKey){syslog(LOG_ERR, "No key = %s\n", key);}
    fclose(fp);
}

int readStringValue(const char* section, char* key, char* val, const char* file)
{
    char sect[SECTION_MAX_LEN];
    //printf("section = %s, key = %s, file = %s\n", section, key, file);
    if (section == NULL || key == NULL || val == NULL || file == NULL)
    {
        syslog(LOG_ERR, "%s: input parameter(s) is NULL!\n", __func__);
        return READ_STR_ERR;
    }
    
    memset(sect, 0, SECTION_MAX_LEN);
    sprintf(sect, "[%s]", section);
    //printf("reading value...\n");
    IniReadValue(sect, key, val, file);
    
    return READ_STR_OK;
}

int readIntValue(const char* section, char* key, const char* file)
{
    char strValue[STRVALUE_MAX_LEN];
    memset(strValue, '\0', STRVALUE_MAX_LEN);
    if(readStringValue(section, key, strValue, file) != READ_STR_OK)
    {
        syslog(LOG_ERR, "%s: error", __func__);
        return 0;
    }
    return(atoi(strValue));
}

void IniWriteValue(const char* section, char* key, char* val, const char* file)
{
    FILE* fp;
    int i = 0, n = 0, err = 0;
    int lineContentLen = 0;
    int position = 0;
    char lineContent[LINE_CONTENT_MAX_LEN];
    char strWrite[LINE_CONTENT_MAX_LEN];
    bool bFoundSection = false;
    bool bFoundKey = false;
    
    memset(lineContent, '\0', LINE_CONTENT_MAX_LEN);
    memset(strWrite, '\0', LINE_CONTENT_MAX_LEN);
    n = sprintf(strWrite, "%s=%s\n", key, val);
    fp = fopen(file, "r+");
    if(fp == NULL)
    {
        syslog(LOG_ERR, "%s: Opent file %s failed.\n", __FILE__, file);
        return;
    }
    while(feof(fp) == 0)
    {
        memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
        fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
        if((lineContent[0] == ';') || (lineContent[0] == '\0') || (lineContent[0] == '\r') || (lineContent[0] == '\n'))
        {
            continue;
        }
        //check section
        if(strncmp(lineContent, section, strlen(section)) == 0)
        {
            bFoundSection = true;
            while(feof(fp) == 0)
            {
                memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
                fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
                //check key
                if(strncmp(lineContent, key, strlen(key)) == 0)
                {
                    bFoundKey = true;
                    syslog(LOG_ERR, "%s: %s=%s\n", __func__, key, val);
                    fseek(fp, (0-strlen(lineContent)),SEEK_CUR);
                    err = fputs(strWrite, fp);
                    if(err < 0){printf("%s err.\n", __func__);}
                    break; 
                }
                else if(lineContent[0] == '[') 
                {
                    break;
                }
            }
            break;
        }
    }
    
    if(!bFoundSection)
    {
        syslog(LOG_ERR, "No section = %s\n", section);
    }
    else if(!bFoundKey)
    {
        syslog(LOG_ERR, "No key = %s\n", key);
    }
    
    fclose(fp);
}

int writeStringVlaue(const char* section, char* key, char* val, const char* file)
{
    char sect[SECTION_MAX_LEN];
    //printf("section = %s, key = %s, file = %s\n", section, key, file);
    if (section == NULL || key == NULL || val == NULL || file == NULL)
    {
        syslog(LOG_ERR, "%s: input parameter(s) is NULL!\n", __func__);
        return READ_STR_ERR;
    }
    memset(sect, '\0', SECTION_MAX_LEN);
    sprintf(sect, "[%s]", section);
    IniWriteValue(sect, key, val, file);

    return READ_STR_OK;
}

int writeIntValue(const char* section, char* key, int val, const char* file)
{
    char strValue[STRVALUE_MAX_LEN];
    memset(strValue, '\0', STRVALUE_MAX_LEN);
    sprintf(strValue, "%d", val);
    
    return writeStringVlaue(section, key, strValue, file);
}

void read_client_data(int &val_instance, int &val_port)
{
    char file[] = "config.ini";
    char section[] = "section";
    char c_instance[] = "instance";
    int t_instance = readIntValue(section, c_instance, file);

    char *c_port = "port";
    int t_port = readIntValue(section, c_port, file);
    syslog(LOG_ERR, "val_instance:%d, val_port:%d\n", val_instance, val_port);
    
    val_instance = t_instance;
    val_port = t_port;
    
    t_instance++;
    t_port++;

    writeIntValue(section, c_instance, t_instance, file);
    writeIntValue(section, c_port, t_port, file);
}

void restore_client_data(int val_instance, int val_port)
{
    char file[] = "config.ini";
    char section[] = "section";
    char c_instance[] = "instance";
    char c_port[] = "c_port";

    writeIntValue(section, c_instance, val_instance, file);
    writeIntValue(section, c_port, val_port, file);
}

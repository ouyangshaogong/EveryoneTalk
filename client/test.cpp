#include "read_config.h"

int main()
{
    char file[] = "config.ini";
    char section[] = "section";
    char c_instance[] = "instance";
    int val_instance = readIntValue(section, c_instance, file);

    //char *c_port = "c_port";
    //int val_port = readIntValue(section, c_port, file);

    val_instance++;
   // val_port++;

    writeIntValue(section, c_instance, val_instance, file);
    printf("writeIntValue c_instance\n");
    //writeIntValue(section, c_port, val_port, file);
    printf("writeIntValue c_port\n");
    return 0;
}

#include "keyboard.h"

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)

//分配内存的大小
#define     SIZE    10
//定义按键们的宏
#define     ESC     "\033"
#define     UP      "\033[A"
#define     DOWN    "\033[B"
#define     LEFT    "\033[D"
#define     RIGHT   "\033[C"


int set_disp_mode(int fd,int option)
{
   int err;
   struct termios term;
   if(tcgetattr(fd,&term)==-1){
     perror("Cannot get the attribution of the terminal");
     return 1;
   }
   if(option)
        term.c_lflag|=ECHOFLAGS;
   else
        term.c_lflag &=~ECHOFLAGS;
   err=tcsetattr(fd,TCSAFLUSH,&term);
   if(err==-1 && err==EINTR){
        perror("Cannot set the attribution of the terminal");
        return 1;
   }
   return 0;
}




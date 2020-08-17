#ifndef __KEY_BOARD__
#define __KEY_BOARD__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

void init_keyboard(void);
void close_keyboard(void);
int kbhit(void);
int readch(void);
int get_key_code();

#endif

/**************************************************************** 
    > Author: Deser
    > e-Mail: yongk202@163.com
    > Date: 2020-07-24 Fri 11:39:21
    > LastEditors: Deser
    > LastEditTime: 2020-07-24 Fri 18:24:32
 ****************************************************************/
#ifndef _HEAD_H
#define _HEAD_H

#define PORT 8888
#define MAX_CLIENT 50

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ws_echo.h"

#endif
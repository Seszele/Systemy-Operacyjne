#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
// include pthread_create
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024
#define MAX_MESSAGE_LENGTH 1024
#define MAX_NAME_LENGTH 512

typedef struct
{
    char *name;
    int available;
    int opponent;
    int fd;
    int still_up;
} client;
#endif
#ifndef CONSTS_H
#define CONSTS_H

#define MAX_LENGTH 1024
#define SERVER_PATH "/"
#define PROJECT_ID 1190
#define MAX_CLIENTS 20 //!

char *const MSG_TYPES[] = {
    "BLANK", "STOP", "LIST", "TOALL", "TOONE", "INIT", "MSG_NACK", "MSG_ERR"};

#define INIT 5
#define LIST 2
#define TOALL 3
#define TOONE 4
#define STOP 1

// message struct for communication between server and client
struct Message
{
    long type;
    int client_id;
    time_t time;
    char data[MAX_LENGTH];
};
#define MSG_SIZE sizeof(struct Message) - sizeof(long)

#endif // CONSTS_H
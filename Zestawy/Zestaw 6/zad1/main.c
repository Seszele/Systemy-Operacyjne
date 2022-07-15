// simple system V message queue server program
// importing libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "consts.h"
// CTRL+C handler sends STOP message to all clients and after receiving ACK from all clients, it exits
int clients[MAX_CLIENTS];
int shutting_down = 0;
int alive = 0;
int global_qid;

void delete_queue()
{
    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    if (key == -1)
    {
        perror("client ftok");
        exit(1);
    }
    // get the message queue id
    int _qid = msgget(key, 0666);
    // delete message queue only if it exists
    if (_qid != -1)
    {
        msgctl(_qid, IPC_RMID, NULL);
    }
}

void sigint_handler(int signo)
{
    shutting_down = 1;
    struct Message msg;
    msg.type = STOP;
    msg.client_id = -2;
    strcpy(msg.data, "server");
    if (alive == 0)
    {
        printf("\033[31m");
        printf("SERVER: All clients disconnected. Shutting down server.\n");
        printf("\033[0m");
        fflush(stdout);
        delete_queue();
        exit(0);
    }
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != 0)
        {
            if (msgsnd(clients[i], &msg, MSG_SIZE, 0) == -1)
            {
                perror("client: error sending stop message");
                exit(1);
            }
        }
    }
}

void server_init(int *qid)
{
    // crate key
    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    // creating message queue
    *qid = msgget(key, IPC_CREAT | 0666);
    if (*qid == -1)
    {
        perror("msgget");
        exit(1);
    }
    // print "success" message in green
    printf("\033[32m");
    printf("SERVER: Successfully created message queue with id: %d\n", *qid);
    printf("\033[0m");
}

void new_client(int qid, char *data)
{
    // find free client slot
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == 0)
        {
            break;
        }
    }
    int cqid = atoi(data);
    // if no free slot,
    if (i == MAX_CLIENTS)
    {
        struct Message msg;
        msg.type = INIT;
        strcpy(msg.data, "NO FREE SLOTS");
        if (msgsnd(cqid, &msg, MSG_SIZE, 0) == -1)
        {
            perror("server: error sending init message");
            exit(1);
        }
        return;
    }
    // add client to list
    clients[i] = cqid;
    // send message to client
    struct Message msg;
    msg.type = INIT;
    sprintf(msg.data, "%d", i);
    if (msgsnd(cqid, &msg, MSG_SIZE, 0) == -1)
    {
        perror("server: error sending init message");
        exit(1);
    }
    alive++;
}

void list_clients(int cqid)
{
    // send message to client with list of clients
    struct Message msg;
    msg.data[0] = '\0';
    msg.type = LIST;
    int i;
    char data[MAX_LENGTH];
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != 0)
        {
            sprintf(data, "%d:%d", i, clients[i]);
            strcat(msg.data, data);
            strcat(msg.data, " ");
        }
    }
    // printf("Im about to send message: %s\n", msg.data);
    if (msgsnd(cqid, &msg, MSG_SIZE, 0) == -1)
    {
        perror("server: error sending list message");
        exit(1);
    }
}

void send_to_all(int id, char *data)
{
    // send message to all clients
    struct Message msg;
    msg.type = TOALL;
    msg.client_id = id;
    msg.time = time(NULL);
    strcpy(msg.data, data);
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != 0)
        {
            if (msgsnd(clients[i], &msg, MSG_SIZE, 0) == -1)
            {
                perror("server: error sending invidual toall message");
                exit(1);
            }
        }
    }
}
void send_to_one(int id, char *data)
{
    // send message to one client
    char *receiver_str = strtok(data, " ");
    char *message_str = strtok(NULL, "\n");
    int receiver = atoi(receiver_str);
    struct Message msg;
    msg.type = TOONE;
    msg.client_id = id;
    msg.time = time(NULL);
    strcpy(msg.data, message_str);
    if (msgsnd(clients[receiver], &msg, MSG_SIZE, 0) == -1)
    {
        perror("server: error sending invidual toone message");
        exit(1);
    }
}
void stop_client(int id, char *data)
{
    // check if client is in list
    if (clients[id] == 0)
    {
        printf("\033[31m");
        printf("SERVER: Client with id %d is not in list\n", id);
        printf("\033[0m");
        fflush(stdout);
        return;
    }
    // send message to client
    if (strcmp(data, "server") != 0)
    {
        struct Message msg;
        msg.type = STOP;
        msg.client_id = id;
        msg.data[0] = '\0';
        msg.time = time(NULL);

        if (msgsnd(clients[id], &msg, MSG_SIZE, 0) == -1)
        {
            perror("server: error sending stop message");
            exit(1);
        }
    }
    clients[id] = 0;
    alive--;
}

int main(int argc, char const *argv[])
{
    int qid;
    // set clients array to 0
    memset(clients, 0, sizeof(clients));
    server_init(&qid);
    global_qid = qid;
    // set CTRL+C handler
    signal(SIGINT, sigint_handler);

    // open file for writing
    int fd = open("server.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    struct Message msg;
    while (1)
    {
        // receive message from clients
        if (msgrcv(qid, &msg, MSG_SIZE, -10, 0) != -1)
        {
            // save timestamp and message to file
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char time_str[100];
            // date and time in format: YYYY-MM-DD HH:MM:SS
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

            char msg_str[MSG_SIZE + 100];
            sprintf(msg_str, "%s | %s | %d | %s\n", time_str, MSG_TYPES[msg.type], msg.client_id > MAX_CLIENTS ? -1 : msg.client_id, msg.type != LIST ? msg.data : "");
            // write to file
            write(fd, msg_str, strlen(msg_str));

            if (msg.data != NULL)
            {
                // in blue
                printf("\033[34m");
                switch (msg.type)
                {
                case INIT:
                    printf("SERVER: Received init command from client with cqid: %d\n", atoi(msg.data));
                    new_client(qid, msg.data);
                    break;
                case LIST:
                    printf("SERVER: Received list command from client with id: %d\n", msg.client_id);
                    list_clients(clients[msg.client_id]);
                    break;
                case TOALL:
                    printf("SERVER: Received toall command from client with id: %d\n", msg.client_id);
                    send_to_all(msg.client_id, msg.data);
                    break;
                case TOONE:
                    printf("SERVER: Received toone command from client with id: %d\n", msg.client_id);
                    send_to_one(msg.client_id, msg.data);
                    break;
                case STOP:
                    printf("SERVER: Received stop command from client with id: %d\n", msg.client_id);

                    stop_client(msg.client_id, msg.data);

                    if (alive == 0 && shutting_down == 1)
                    {
                        // red
                        printf("\033[31m");
                        printf("SERVER: All clients disconnected. Shutting down server.\n");
                        printf("\033[0m");
                        fflush(stdout);
                        delete_queue();
                        return 0;
                    }
                    break;
                default:
                    printf("SERVER: received command of unknown type: %ld -> %s\n", msg.type, msg.data);
                    break;
                }
                printf("\033[0m");
                fflush(stdout);
            }
        }

        // puts("server: next command");
    }
    delete_queue();
    return 0;
}
// simple system V message queue client program
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
#include <ctype.h>
#include "consts.h"

int global_id = -1;
int global_sqid = -1;
int global_qid = -1;
void delete_queue()
{
    key_t key = ftok(getenv("HOME"), getpid());
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
//  SIGINT signal handler
void sigint_handler(int signo)
{
    struct Message msg;
    msg.type = STOP;
    msg.client_id = global_id;
    if (msgsnd(global_sqid, &msg, MSG_SIZE, 0) == -1)
    {
        perror("client: error sending stop message");
        delete_queue();
        exit(1);
    }
    fflush(stdout);
}

void client_init(int *sqid, int *qid, int *id)
{
    // crate client key
    key_t key = ftok(getenv("HOME"), getpid());
    if (key == -1)
    {
        perror("client ftok");
        delete_queue();
        exit(1);
    }
    // creating message queue
    *qid = msgget(key, IPC_CREAT | 0666);
    if (*qid == -1)
    {
        perror("client msgget");
        delete_queue();
        exit(1);
    }

    // get server queue id
    key = ftok(getenv("HOME"), PROJECT_ID);
    if (key == -1)
    {
        perror("server ftok");
        delete_queue();
        exit(1);
    }
    *sqid = msgget(key, IPC_CREAT | 0666);
    if (*sqid == -1)
    {
        perror("server msgget");
        delete_queue();
        exit(1);
    }

    // send message to server
    struct Message msg;
    msg.type = INIT;
    sprintf(msg.data, "%d", *qid);
    if (msgsnd(*sqid, &msg, MSG_SIZE, 0) == -1)
    {
        fprintf(stderr, "client: error sending init message, sqid: %d, qid: %d", *sqid, *qid);
        perror("");
        delete_queue();
        exit(1);
    }
    // receive id message from server
    if (msgrcv(*qid, &msg, MSG_SIZE, INIT, 0) == -1)
    {
        perror("client: error receiving init message");
        delete_queue();
        exit(1);
    }
    // check if message is NO FREE SLOTS
    if (strcmp(msg.data, "NO FREE SLOTS") == 0)
    {
        // red
        printf("\033[31m");
        printf("CLIENT: No free slots on the server! Shutting down\n");
        printf("\033[0m");
        fflush(stdout);
        delete_queue();
        exit(1);
    }
    *id = atoi(msg.data);

    // print "success" message in green
    printf("\033[32m");
    printf("CLIENT: Successfully initialised! qid: %d, sqid: %d, id: %d\n", *qid, *sqid, *id);
    printf("\033[0m");
    fflush(stdout);
}

void handle_input(char *input, int sqid, int id)
{
    // send message to server based on input
    struct Message msg;
    // get first word of input
    char *first_word = strtok(input, " ");
    if (strcmp(first_word, "list\n") == 0)
    {
        // send list command to server
        msg.type = LIST;
        msg.client_id = id;
        if (msgsnd(sqid, &msg, MSG_SIZE, 0) == -1)
        {
            perror("client: error sending list message");
            delete_queue();
            exit(1);
        }
    }
    else if (strcmp(first_word, "toall") == 0)
    {
        // send 2all command to server
        char *message = strtok(NULL, "\n");
        // check if message is empty
        if (message == NULL)
        {
            message = "";
        }
        // printf("message: %s\n", message);
        msg.type = TOALL;
        msg.client_id = id;
        strcpy(msg.data, message);
        if (msgsnd(sqid, &msg, MSG_SIZE, 0) == -1)
        {
            perror("client: error sending toall message");
            delete_queue();
            exit(1);
        }
    }
    else if (strcmp(first_word, "toone") == 0)
    {
        // send 2one command to server
        char *receiver_str = strtok(NULL, " ");
        // check if receiver is empty
        if (receiver_str == NULL || !isdigit(receiver_str[0]))
        {
            // yellow
            printf("\033[33m");
            printf("CLIENT: No receiver specified!\n");
            printf("\033[0m");
            fflush(stdout);
            return;
        }
        int receiver = atoi(receiver_str);

        char *message = strtok(NULL, "\n");
        // printf("receiver: %s, message: %s\n", receiver_str, message);
        if (message == NULL)
        {
            printf("\033[33m");
            printf("CLIENT: No message!\n");
            printf("\033[0m");
            fflush(stdout);
            return;
        }
        // printf("receiver: %s, message: %s\n", receiver_str, message);
        msg.type = TOONE;
        msg.client_id = id;
        sprintf(msg.data, "%d %s", receiver, message);
        if (msgsnd(sqid, &msg, MSG_SIZE, 0) == -1)
        {
            perror("client: error sending toone message");
            delete_queue();
            exit(1);
        }
    }
    else if (strcmp(first_word, "stop\n") == 0)
    {
        // send stop command to server
        msg.type = STOP;
        msg.client_id = id;
        msg.data[0] = '\0';
        if (msgsnd(sqid, &msg, MSG_SIZE, 0) == -1)
        {
            perror("client: error sending stop message");
            delete_queue();
            exit(1);
        }
    }
}
int check_input()
{
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(0, &fds);
}
int main(int argc, char const *argv[])
{
    int qid, sqid, id;
    client_init(&sqid, &qid, &id);
    global_sqid = sqid;
    global_qid = qid;
    global_id = id;
    // catch SIGINT signal
    signal(SIGINT, sigint_handler);

    char input[MAX_LENGTH];
    struct Message msg;

    while (1)
    {
        // send message to server
        if (check_input())
        {
            fgets(input, MAX_LENGTH, stdin);
            handle_input(input, sqid, id);
        }

        // receive message back from server
        if (msgrcv(qid, &msg, MSG_SIZE, 0, IPC_NOWAIT) != -1)
        {
            struct tm *tm = localtime(&msg.time);
            char time_str[100];
            // date and time in format: YYYY-MM-DD HH:MM:SS
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

            switch (msg.type)
            {
            case LIST:
                printf("List of clients:\n%s\n", msg.data);
                break;
            case TOALL:
            case TOONE:
                printf("%s: Message from %d:\n%s\n", time_str, msg.client_id, msg.data);
                break;
            case STOP:
                printf("\033[31m");
                // if it came from server shutdown - send confirmation back ( and do nicer display)
                if (strcmp(msg.data, "server") == 0)
                {
                    msg.client_id = id;
                    printf("Server initiated shutdown - client stopping!\n");
                    if (msgsnd(sqid, &msg, MSG_SIZE, 0) == -1)
                    {
                        perror("client: error sending stop message");
                        delete_queue();
                        exit(1);
                    }
                }
                else
                {
                    printf("Client stopping!\n");
                }
                printf("\033[0m");
                fflush(stdout);
                delete_queue();
                return 0;
            default:
                printf("CLIENT: Unknown %ld", msg.type);
                break;
            }
        }
    }
    delete_queue();

    return 0;
}
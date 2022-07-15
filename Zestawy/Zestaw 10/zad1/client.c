#include "shared.h"
int server_socket_fd;
char symbol = '?';
int your_turn = 0;
int opponent_move = -1;
char board[9];
char name[MAX_NAME_LENGTH];
char status[50] = "Couldn't resolve the game";
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// sigint handler
void sigint_handler(int signum)
{
    // send logout to server
    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "logout|%s", name);
    send(server_socket_fd, message, MAX_MESSAGE_LENGTH, 0);
    // close socket
    close(server_socket_fd);
    // exit
    exit(0);
}

void connect_local(const char *path)
{
    server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr_local;
    addr_local.sun_family = AF_UNIX;
    strcpy(addr_local.sun_path, path);
    if (connect(server_socket_fd, (struct sockaddr *)&addr_local, sizeof(addr_local)) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("local client connected on %s\n", path);
}

void connect_remote(char *host, const char *port)
{
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in struct_remote;
    struct_remote.sin_family = AF_INET;
    struct_remote.sin_port = htons(atoi(port));
    struct_remote.sin_addr.s_addr = inet_addr(host);
    if (connect(server_socket_fd, (struct sockaddr *)&struct_remote, sizeof(struct_remote)) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("remote client connected on %s\n", host);
}

void draw()
{
    printf("\n");
    printf("%c  | %c | %c\n", board[0], board[1], board[2]);
    printf("---|---|---\n");
    printf("%c  | %c | %c\n", board[3], board[4], board[5]);
    printf("---|---|---\n");
    printf("%c  | %c | %c\n", board[6], board[7], board[8]);
    printf("\n");
}

// checks if symbol won in tic tac toe
void check_game_status()
{
    char *msg = malloc(MAX_MESSAGE_LENGTH + 1);
    sprintf(msg, "game|%s|won", name);
    if (board[0] == symbol && board[1] == symbol && board[2] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");
        // exit(0);
    }
    if (board[3] == symbol && board[4] == symbol && board[5] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");
        // exit(0);
    }
    if (board[6] == symbol && board[7] == symbol && board[8] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");
        // exit(0);
    }
    if (board[0] == symbol && board[3] == symbol && board[6] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");

        // exit(0);
    }
    if (board[1] == symbol && board[4] == symbol && board[7] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");

        // exit(0);
    }
    if (board[2] == symbol && board[5] == symbol && board[8] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");

        // exit(0);
    }
    if (board[0] == symbol && board[4] == symbol && board[8] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");

        // exit(0);
    }
    if (board[2] == symbol && board[4] == symbol && board[6] == symbol)
    {
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You won!");

        // exit(0);
    }
    free(msg);
    // check for draw
    int draw = 0;
    for (int i = 0; i < 9; i++)
    {
        if (board[i] == 'X' || board[i] == 'O')
        {
            draw++;
        }
    }
    if (draw == 9)
    {
        sprintf(msg, "game|%s|draw", name);
        send(server_socket_fd, msg, strlen(msg), 0);
        strcpy(status, "You both lost!");

        // exit(0);
    }
}

void start_game()
{
    // printf("games tarsted\n");
    while (1)
    {
        if (your_turn)
        {
            // draw
            // choose where to place
            // send
            // wait for response - your_turn = false

            draw();
            printf("%c Choose where to place <1-9>\n", symbol);
            int choice;
            scanf("%d", &choice);
            if (board[choice - 1] != 'X' && board[choice - 1] != 'O')
            {
                board[choice - 1] = symbol;
                char *message = malloc(MAX_MESSAGE_LENGTH + 1);
                sprintf(message, "move|%s|%d", name, choice);
                send(server_socket_fd, message, MAX_MESSAGE_LENGTH, 0);
                free(message);
                your_turn = 0;
                draw();
                printf("Waiting for opponent move...\n");
                // check if won i guess?
                check_game_status();
            }
            else
            {
                printf("That place is already taken\n");
                continue;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    // sigint handler
    signal(SIGINT, sigint_handler);
    // set board
    for (int i = 0; i < 9; i++)
    {
        // board[i] = i to char
        board[i] = (i + 1) + '0';
    }
    if (argc != 4)
    {
        printf("Usage: %s <name> <connection_type> <remote_port//local_path>\n", argv[0]);
        exit(1);
    }
    if (strcmp(argv[2], "local") == 0)
    {
        connect_local(argv[3]);
    }
    else if (strcmp(argv[2], "remote") == 0)
    {
        connect_remote("127.0.0.1", argv[3]);
    }
    else
    {
        printf("<connection_type> should be \"local\" or \"remote\"\n");
        exit(1);
    }
    strcpy(name, argv[1]);
    // send "add|<name>" to server
    char *message = malloc(MAX_MESSAGE_LENGTH + 1);
    sprintf(message, "login|%s", argv[1]);
    send(server_socket_fd, message, strlen(message), 0);
    // while, message in format <command><argument>
    // on login|success print waiting for opponent
    while (1)
    {
        int bytes_read = recv(server_socket_fd, message, MAX_MESSAGE_LENGTH, 0);
        if (bytes_read == -1)
        {
            perror("recv");
            exit(1);
        }
        if (bytes_read == 0)
        {
            puts(status);
            printf("server closed connection\n");
            exit(0);
        }
        message[bytes_read] = '\0';
        // printf("%s\n", message);
        // strtoke the message by '|'
        char *command = strtok(message, "|");
        char *argument = strtok(NULL, "|");
        if (strcmp(command, "login") == 0)
        {
            if (strcmp(argument, "success") == 0)
            {
                printf("Succesfully connected to server\n");
            }
            else if (strcmp(argument, "waiting") == 0)
            {
                printf("Waiting for opponent...\n");
            }
            else if (strcmp(argument, "taken") == 0)
            {
                printf("%s is already taken!\n", argv[1]);
                exit(1);
            }
            else if (strcmp(argument, "full") == 0)
            {
                printf("Server is full, try again later\n");
                exit(1);
            }
            else
            {
                printf("unknown message\n");
            }
        }
        else if (strcmp(command, "game") == 0)
        {
            symbol = argument[0];
            printf("Opponent found!\nStarting game as %c\n", symbol);
            // O should go first, TODO add global variable
            your_turn = (symbol == 'O');
            pthread_t thread;
            pthread_create(&thread, NULL, (void *(*)(void *))start_game, NULL);
        }
        else if (strcmp(command, "move") == 0)
        {
            printf("Opponent moved!\n");
            int choice = atoi(argument);
            board[choice - 1] = symbol == 'X' ? 'O' : 'X';
            your_turn = 1;
        }
        else if (strcmp(command, "end") == 0)
        {
            if (strcmp(argument, "draw") == 0)
            {
                strcpy(status, "You both lost!");
            }
            else if (strcmp(argument, "won") == 0)
            {
                strcpy(status, "You lost!");
            }
        }
        else if (strcmp(command, "ping") == 0)
        {
            // send pong
            char *message = malloc(MAX_MESSAGE_LENGTH + 1);
            sprintf(message, "pong|%s| ", name);
            send(server_socket_fd, message, strlen(message), 0);
            free(message);
        }
        else
        {
            printf("unknown message\n");
        }
    }
    return 0;
}

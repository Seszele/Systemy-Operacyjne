#include "shared.h"

int socket_fd_local;
struct sockaddr_un addr_local;
int socket_fd_remote;
struct sockaddr_in struct_remote;

client *clients[MAX_CLIENTS];
int current_clients = 0;

void start_local_server(const char *path)
{
    socket_fd_local = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd_local == -1)
    {
        perror("socket");
        exit(1);
    }

    addr_local.sun_family = AF_UNIX;
    strcpy(addr_local.sun_path, path);
    unlink(path);
    if (bind(socket_fd_local, (struct sockaddr *)&addr_local, sizeof(addr_local)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(socket_fd_local, 5) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("local server started on %s\n", path);
}

void start_remote_server(const char *port)
{
    socket_fd_remote = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_remote == -1)
    {
        perror("socket");
        exit(1);
    }

    struct_remote.sin_family = AF_INET;
    struct_remote.sin_port = htons(atoi(port));
    struct_remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(socket_fd_remote, (struct sockaddr *)&struct_remote, sizeof(struct_remote)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(socket_fd_remote, 5) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("remote server started on %s\n", port);
}

int get_client_fd(const char *path, const char *port)
{
    int client_fd = -1;
    struct pollfd *fds = calloc(2 + current_clients, sizeof(struct pollfd));
    fds[0].fd = socket_fd_local;
    fds[0].events = POLLIN;
    fds[1].fd = socket_fd_remote;
    fds[1].events = POLLIN;
    // TODO MUTEX
    for (int i = 0; i < current_clients; i++)
    {
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    int ret = poll(fds, 2 + current_clients, -1);
    if (ret == -1)
    {
        perror("poll");
        exit(1);
    }
    for (int i = 0; i < 2 + current_clients; i++)
    {
        if (fds[i].revents & POLLIN)
        {
            client_fd = fds[i].fd;
            break;
        }
    }
    if (client_fd == socket_fd_local || client_fd == socket_fd_remote)
    {
        client_fd = accept(client_fd, NULL, NULL);
        if (client_fd == -1)
        {
            perror("accept");
            exit(1);
        }
    }
    // TODO else?
    free(fds);
    return client_fd;
}

client *init_client(char *name, int fd)
{
    client *new_client = calloc(1, sizeof(client));
    new_client->fd = fd;
    new_client->available = 1;
    new_client->still_up = 1;
    new_client->name = calloc(MAX_NAME_LENGTH, sizeof(char));
    strcpy(new_client->name, name);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == NULL)
        {
            clients[i] = new_client;
            break;
        }
    }
    // send confirmation
    send(fd, "login|success", MAX_MESSAGE_LENGTH, 0);
    current_clients++;
    return new_client;
}

void login(char *name, int client_fd)
{
    // check if there is space for new client'
    if (current_clients == MAX_CLIENTS)
    {
        send(client_fd, "login|full", MAX_MESSAGE_LENGTH, 0);
        close(client_fd);
        return;
    }
    // check if name is already taken
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
        {
            send(client_fd, "login|taken", MAX_MESSAGE_LENGTH, 0);
            close(client_fd);
            return;
        }
    }
    // check if there is an available client
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != NULL && clients[i]->available == 1)
        {
            printf("Matched %s with %s\n", clients[i]->name, name);
            // login client and start the game
            client *new_client = init_client(name, client_fd);

            clients[i]->available = 0;
            clients[i]->opponent = client_fd;
            new_client->available = 0;
            new_client->still_up = 1;
            new_client->opponent = clients[i]->fd;

            // start game of tic tac toe
            // choose random player
            int random = rand() % 2;
            if (random == 0)
            {
                send(clients[i]->fd, "game|X", MAX_MESSAGE_LENGTH, 0);
                send(new_client->fd, "game|O", MAX_MESSAGE_LENGTH, 0);
            }
            else
            {
                send(clients[i]->fd, "game|O", MAX_MESSAGE_LENGTH, 0);
                send(new_client->fd, "game|X", MAX_MESSAGE_LENGTH, 0);
            }

            return;
        }
    }
    // if no opponent is available, simply add client as waiting
    printf("%s waiting\n", name);
    init_client(name, client_fd);
    // send confirmation
    send(client_fd, "login|waiting", MAX_MESSAGE_LENGTH, 0);
}
int get_opponent_fd(int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != NULL && clients[i]->fd == client_fd)
        {
            return clients[i]->opponent;
        }
    }
    return -1;
}

int get_client_fd_by_name(char *name)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
        {
            return clients[i]->fd;
        }
    }
    return -1;
}
void logout(int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != NULL && clients[i]->fd == client_fd)
        {
            free(clients[i]);
            clients[i] = NULL;
            close(client_fd);
            current_clients--;
            return;
        }
    }
}

void *ping()
{
    while (1)
    {
        // remove clients that are not up
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != NULL && clients[i]->still_up == 0)
            {
                printf("❗ Removing %s - not responding\n", clients[i]->name);
                close(clients[i]->fd);
                free(clients[i]);
                clients[i] = NULL;
                current_clients--;
            }
        }
        // ping remaining clients
        // printf("Ping\n");
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != NULL)
            {
                // printf("Pinging %s\n", clients[i]->name);
                clients[i]->still_up = 0;
                send(clients[i]->fd, "ping| ", MAX_MESSAGE_LENGTH, 0);
            }
        }
        // wait for ping response
        sleep(8);
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    if (argc != 3)
    {
        printf("usage: %s <local_path> <remote_port>\n", argv[0]);
        exit(1);
    }
    start_local_server(argv[1]);
    start_remote_server(argv[2]);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, (void *)ping, NULL);

    while (1)
    {
        int client_fd = get_client_fd(argv[1], argv[2]);
        if (client_fd == -1)
        {
            // puts("cont");
            continue;
        }
        // read message
        char *message = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        int ret = read(client_fd, message, MAX_MESSAGE_LENGTH);
        if (ret == -1)
        {
            perror("read");
            exit(1);
        }
        if (ret == 0)
        {
            printf("client disconnected: %d\n", client_fd);
            close(client_fd);
            continue;
        }
        // printf("message: %s\n", message);
        // strtok the message by '|'
        char *command = strtok(message, "|");
        char *name = strtok(NULL, "|");
        char *argument = strtok(NULL, "|");

        // execute command
        if (strcmp(command, "login") == 0)
        {
            // login
            login(name, client_fd);
        }
        else if (strcmp(command, "logout") == 0)
        {
            // logout the opponent as well
            int opponent_fd = get_opponent_fd(client_fd);
            if (opponent_fd != -1)
            {
                logout(opponent_fd);
            }
            logout(client_fd);
        }
        else if (strcmp(command, "move") == 0)
        {
            // send move to opponent
            int opponent_fd = get_opponent_fd(client_fd);
            if (opponent_fd == -1)
            {
                send(client_fd, "move|error", MAX_MESSAGE_LENGTH, 0);
                continue;
            }
            sprintf(message, "move|%s", argument);
            send(opponent_fd, message, MAX_MESSAGE_LENGTH, 0);
        }
        else if (strcmp(command, "game") == 0)
        {
            if (strcmp(argument, "won") == 0 || strcmp(argument, "draw") == 0)
            {
                printf("%s %s\n", name, argument);
                int opponent_fd = get_opponent_fd(client_fd);
                if (opponent_fd == -1)
                {
                    send(client_fd, "game|error", MAX_MESSAGE_LENGTH, 0);
                    continue;
                }
                sprintf(message, "end|%s", argument);
                send(opponent_fd, message, MAX_MESSAGE_LENGTH, 0);
                logout(client_fd);
                logout(opponent_fd);
            }
        }
        else if (strcmp(command, "pong") == 0)
        {
            // set client as still_up
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] != NULL && clients[i]->fd == client_fd)
                {
                    clients[i]->still_up = 1;
                }
            }
        }
        else
        {
            printf("unknown command: %s\n", command);
        }
    }
    return 0;
}

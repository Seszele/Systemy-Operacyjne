#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <limits.h>
#include <fcntl.h>
void free_lines(char **lines, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(lines[i]);
    }
    free(lines);
}
void write_lines(int fd, char **lines, int n)
{
    // append newline for each line (realloc space for newline)
    for (int i = 0; i < n; i++)
    {
        lines[i] = realloc(lines[i], strlen(lines[i]) + 2);
        lines[i][strlen(lines[i])] = '\n';
        lines[i][strlen(lines[i]) + 1] = '\0';
    }

    for (int i = 0; i < n; i++)
    {
        if (write(fd, lines[i], strlen(lines[i])) != strlen(lines[i]))
        {
            error(1, errno, "write error");
        }
    }
}
// function that allocates memory to lines
char **alloc_lines(int n)
{
    char **lines = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        lines[i] = malloc(sizeof(char));
        if (lines[i] == NULL)
        {
            error(1, errno, "malloc error");
        }
        lines[i][strlen(lines[i])] = '\0';
    }
    return lines;
}

void split_and_save(char *msg, char **lines)
{
    // split msg by ':' and save to lines array
    char *token = strtok(msg, ":");
    while (token != NULL)
    {
        int line_index = token[0] - '0';
        token++;

        lines[line_index] = realloc(lines[line_index], strlen(lines[line_index]) + strlen(token) + 1);
        if (lines[line_index] == NULL)
        {
            error(1, errno, "realloc error");
        }
        strcat(lines[line_index], token);
        token = strtok(NULL, ":");
    }
}
void read_from_fifo(int fifo_fd, int dest_fd, int N)
{
    // read from fifo, split by ':' and write to dest_fd
    char **lines = alloc_lines(10);
    char buf[N + 3];
    char whole_msg[PATH_MAX];
    memset(buf, '\0', N + 3);
    memset(whole_msg, '\0', PATH_MAX);

    int n;
    // int other_index = 0;
    while ((n = read(fifo_fd, buf, N + 2)) > 0)
    {
        // int line_index = buf[0] - '0';
        // append whole_msg with buf
        strcat(whole_msg, buf);
        memset(buf, '\0', N + 3);
    }
    if (n < 0)
    {
        error(1, errno, "read error");
    }
    split_and_save(whole_msg, lines);
    write_lines(dest_fd, lines, 10);
    free_lines(lines, 10);
}

int main(int argc, char const *argv[])
{
    // check if 3 aguments passed
    if (argc != 4)
    {
        printf("Wrong number of arguments\n");
        exit(1);
    }
    // open fifo argv[1] read
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening fifo\n");
        exit(1);
    }
    // open destination file argv[2] write
    int dest_fd = open(argv[2], O_WRONLY);
    if (dest_fd == -1)
    {
        printf("Error opening destination file\n");
        exit(1);
    }
    int N = atoi(argv[3]);
    // print start in red
    printf("\033[0;31mConsument begins\033[0m\n");
    read_from_fifo(fd, dest_fd, N);
    // print completed in green
    printf("\033[0;32mCompleted\033[0m\n");
    return 0;
}

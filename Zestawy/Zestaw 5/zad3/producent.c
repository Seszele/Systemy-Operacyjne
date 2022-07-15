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

// funtion to wait random time in seconds
void wait_random_time(int min, int max)
{
    int r = rand() % (max - min + 1) + min;
    sleep(r);
}

// function that reads N chars (or to end of file) from file descriptor and sends them to fifo
void read_from_file(int fd, int fifo_fd, int line_index, int N)
{
    char buf[N + 1];
    char msg[N + 3];
    memset(buf, '\0', N + 1);
    memset(msg, '\0', N + 3);
    int n;
    while ((n = read(fd, buf, N)) > 0)
    {
        // printf("%d buf: %s\n", n, buf);
        sprintf(msg, "%d%s:", line_index, buf);

        if (write(fifo_fd, msg, strlen(msg)) != strlen(msg))
        {
            error(1, errno, "write error!");
        }
        memset(buf, '\0', N + 1);
        memset(msg, '\0', N + 3);

        wait_random_time(1, 1);
    }
    if (n < 0)
    {
        error(1, errno, "read error");
    }
}

int main(int argc, char const *argv[])
{
    // check if 4 aguments passed
    if (argc != 5)
    {
        printf("Wrong number of arguments\n");
        exit(1);
    }
    // open fifo argv[1] write
    int fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
        printf("Error opening fifo\n");
        exit(1);
    }
    int line_index = argv[2][0] - '0';
    // open read only text file from argv[3]

    int fd_read = open(argv[3], O_RDONLY);
    if (fd_read == -1)
    {
        printf("Error opening file\n");
        exit(1);
    }
    int N = atoi(argv[4]);
    printf("\033[0;34mProducent begins %d\033[0m\n", line_index);

    read_from_file(fd_read, fd, line_index, N);

    return 0;
}

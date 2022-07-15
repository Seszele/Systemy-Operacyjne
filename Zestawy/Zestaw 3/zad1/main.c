#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        puts("potrzebny 1 argument! ERR");
        return 0;
    }
    int n = atoi(argv[1]);
    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // puts("co");
            printf("pid: %d\n", getpid());
            return 0;
        }
    }

    return 0;
}
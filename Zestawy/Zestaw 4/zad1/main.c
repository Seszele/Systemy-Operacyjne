#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum)
{
    printf("Obsluga sygnalu SIGUSR1\n");
}

int main(int argc, char const *argv[])
{
    sigset_t new_set, pending_set;
    switch (argv[1][0] - '0')
    {
    case 0:
        puts("\tignore");
        signal(SIGUSR1, SIG_IGN);
        break;
    case 1:
        puts("\thandler");
        signal(SIGUSR1, handler);
        break;
    case 2:
        puts("\tmask");
        sigemptyset(&new_set);
        sigaddset(&new_set, SIGUSR1);
        sigprocmask(SIG_SETMASK, &new_set, NULL);
        break;
    case 3:
        puts("\tpending");
        sigemptyset(&new_set);
        sigaddset(&new_set, SIGUSR1);
        sigprocmask(SIG_SETMASK, &new_set, NULL);
        break;
    default:
        puts("Need 1 argument - [0,3]");
        return 0;
    }
    printf("Parent:\n");
    raise(SIGUSR1);

    if (argv[1][0] - '0' == 3)
    {
        sigpending(&pending_set);
        if (sigismember(&pending_set, SIGUSR1))
        {
            puts("SIGUSR1 is pending...");
        }
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        puts("Child:");
        if (argv[1][0] - '0' == 3)
        {
            sigpending(&pending_set);
            if (sigismember(&pending_set, SIGUSR1))
            {
                puts("SIGUSR1 is pending...");
            }
        }
        else
        {
            raise(SIGUSR1);
        }
        exit(0);
    }

    char *args[] = {"./EXEC", (char *const)argv[1], NULL};
    execvp(args[0], args);

    return 0;
}

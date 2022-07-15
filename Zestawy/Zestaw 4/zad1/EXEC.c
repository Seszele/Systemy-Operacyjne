#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    sigset_t pending_set;
    puts("EXEC:");
    if (argv[1][0] - '0' == 1)
    {
        puts("Brak testu");
        return 0;
    }
    else if (argv[1][0] - '0' == 3)
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
    return 0;
}

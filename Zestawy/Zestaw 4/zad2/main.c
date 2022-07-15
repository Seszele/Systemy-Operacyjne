#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
int SIGUSR2_ran = 0;
void reset_handler(int sig_no)
{
    printf("Got signal %d in handler\n", sig_no);
}
void nodefer_handler(int sig_no)
{
    if (SIGUSR2_ran < 1)
    {
        SIGUSR2_ran++;
        puts("Raising SIGUSR2 inside handler - without the SA_NODEFER the signal would not be caught by handler");
        raise(SIGUSR2);
        while (SIGUSR2_ran < 1)
            ;
    }
}
void info_handler(int sig_no, siginfo_t *siginfo, void *n)
{
    printf("Signal no: %d\npid: %d\nSending process ID: %d\n", sig_no, siginfo->si_pid, siginfo->si_pid);
    printf("Real user ID of sending process: %d\nSignal code: %d\n", siginfo->si_uid, siginfo->si_code);
}

int main(int argc, char const *argv[])
{
    struct sigaction act, act2, act3;
    sigemptyset(&act.sa_mask);
    sigemptyset(&act2.sa_mask);
    sigemptyset(&act3.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act2.sa_flags = SA_RESETHAND;
    act3.sa_flags = SA_NODEFER;

    act.sa_sigaction = info_handler;
    act2.sa_handler = reset_handler;
    act3.sa_handler = nodefer_handler;
    sigaction(SIGCONT, &act, NULL);
    sigaction(SIGUSR1, &act2, NULL);
    sigaction(SIGUSR2, &act3, NULL);

    printf("*SA_SIGINFO - SIGCONT*\n");
    raise(SIGCONT);

    printf("\n*SA_NODEFER - SIGUSR2*\n");
    raise(SIGUSR2);

    printf("\n*SA_RESETHAND - SIGUSR1*\n");
    raise(SIGUSR1);
    printf("Second raise() - should be default:\n");
    raise(SIGUSR1);

    return 0;
}

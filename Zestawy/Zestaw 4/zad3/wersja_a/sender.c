#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
int received = 0;
pid_t catcher_pid = 0;
int catcher_received = 0;
int got_sig2 = 0;
void handler(int sig_no, siginfo_t *info, void *ucontext)
{
    if (sig_no == SIGUSR1 || sig_no == SIGRTMIN)
    {
        received++;
    }
    if (sig_no == SIGUSR2 || sig_no == (SIGRTMIN + 1))
    {
        got_sig2 = 1;
        catcher_received = info->si_value.sival_int;
    }
}
void send(int mode, int sig_no, int val)
{
    int sig = 0;

    switch (mode)
    {
    case 0:
        kill(catcher_pid, sig_no);
        break;
    case 1:
        sigqueue(catcher_pid, sig_no, (const union sigval){.sival_int = val});
        break;
    case 2:
        sig = (sig_no == SIGUSR1) ? SIGRTMIN : SIGRTMIN + 1;
        kill(catcher_pid, sig);
        break;

    default:
        puts("needs [0-2] range of args");
        return;
        break;
    }
}
int main(int argc, char const *argv[])
{

    sigset_t temp_mask;
    sigfillset(&temp_mask);
    sigdelset(&temp_mask, SIGUSR1);
    sigdelset(&temp_mask, SIGUSR2);
    sigdelset(&temp_mask, SIGRTMIN);
    sigdelset(&temp_mask, SIGRTMIN + 1);
    sigprocmask(SIG_SETMASK, &temp_mask, NULL);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);
    sigaction((SIGRTMIN + 1), &act, NULL);

    catcher_pid = atoi(argv[1]);
    int sig_count = atoi(argv[2]);

    for (int i = 0; i < sig_count; i++)
    {
        send(atoi(argv[3]), SIGUSR1, 1);
    }
    send(atoi(argv[3]), SIGUSR2, 1);

    while (got_sig2 == 0)
    {
        sigsuspend(&temp_mask);
    }

    if (atoi(argv[3]) == 1)
    {
        printf("Sender info: catcher received: %d\n", catcher_received);
    }

    printf("Sender received: %d/%d\n", received, sig_count);
    return 0;
}

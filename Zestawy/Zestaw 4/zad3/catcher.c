#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
int received = 0;
pid_t sender_pid = 0;
int mode = 0;
int got_sig2 = 0;

void send(int mode, int sig_no, int val)
{
    int sig = 0;
    switch (mode)
    {
    case 0:
        kill(sender_pid, sig_no);
        break;
    case 1:
        sigqueue(sender_pid, sig_no, (const union sigval){.sival_int = val});
        break;
    case 2:
        sig = (sig_no == SIGUSR1) ? SIGRTMIN : (SIGRTMIN + 1);
        kill(sender_pid, sig);
        break;

    default:
        puts("needs [0-2] range of args");
        return;
        break;
    }
}
void handler(int sig_no, siginfo_t *info, void *ucontext)
{
    sender_pid = info->si_pid;
    mode = info->si_value.sival_int;
    if (mode == 0 && (sig_no == SIGRTMIN || sig_no == SIGRTMIN + 1))
    {
        mode = 2;
    }

    if (sig_no == SIGUSR1 || sig_no == SIGRTMIN)
    {
        send(mode, SIGUSR1, 1);
        received++;
    }
    else if (sig_no == SIGUSR2 || sig_no == SIGRTMIN + 1)
    {
        got_sig2 = 1;
        send(mode, SIGUSR1, 1);
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
    sigaction(SIGRTMIN + 1, &act, NULL);

    while (got_sig2 == 0)
    {
        sigsuspend(&temp_mask);
    }

    for (int i = 0; i < received; i++)
    {
        send(mode, SIGUSR1, i);
    }
    send(mode, SIGUSR2, received);

    printf("Catcher received: %d\n", received);

    return 0;
}

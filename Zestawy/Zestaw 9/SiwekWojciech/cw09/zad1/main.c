#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "pthread.h"

int christmas = 0;
int waiting_raindeers = 0;
pthread_mutex_t r_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t r_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t raindeers_ready_cond = PTHREAD_COND_INITIALIZER;

void *raindeer_routine(void *arg)
{
    int id = *(int *)arg;
    // set seed to thread id
    srand(pthread_self());

    while (1)
    {
        // wait till waiting_raindeers is 0
        pthread_mutex_lock(&r_wait_mutex);
        while (waiting_raindeers > 0)
        {
            pthread_cond_wait(&r_cond, &r_wait_mutex);
        }
        pthread_mutex_unlock(&r_wait_mutex);

        // wait 5-10 seconds
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&r_mutex);
        waiting_raindeers++;
        // wypisz Komunikat: Renifer: czeka _ reniferów na Mikołaja, ID), jeśli jest dziewiątym reniferem to wybudza Mikołaja (Komunikat: Renifer: wybudzam Mikołaja, ID
        printf("\033[32m");
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", waiting_raindeers, id);
        printf("\033[0m");
        fflush(stdout);
        if (waiting_raindeers == 9)
        {
            printf("\033[1;34m");
            printf("Renifer: wybudzam Mikołaja, ID: %d\n", id);
            printf("\033[0m");
            fflush(stdout);
            pthread_mutex_lock(&s_mutex);
            pthread_cond_signal(&raindeers_ready_cond);
            pthread_mutex_unlock(&s_mutex);
        }
        pthread_mutex_unlock(&r_mutex);
    }

    return arg;
}

void *santa_routine(void *arg)
{
    srand(pthread_self());
    while (1)
    {
        // wait till woken up by raindeers
        pthread_mutex_lock(&s_mutex);
        while (waiting_raindeers < 9)
        {
            pthread_cond_wait(&raindeers_ready_cond, &s_mutex);
        }
        pthread_mutex_unlock(&s_mutex);

        // red
        printf("\033[31m");
        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        printf("\033[0m");
        fflush(stdout);

        // wait 2-4 seconds
        sleep(rand() % 3 + 2);

        pthread_mutex_lock(&r_wait_mutex);
        waiting_raindeers = 0;
        pthread_cond_broadcast(&r_cond);
        pthread_mutex_unlock(&r_wait_mutex);

        printf("\033[31m");
        christmas++;
        printf("Mikołaj: dostarczylem zabawki, %d\n", christmas);
        printf("\033[0m");
        fflush(stdout);

        if (christmas == 3)
            break;
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    printf("Starting\n");
    int n = 9;
    pthread_t *threads = malloc(10 * sizeof(pthread_t));
    int *ids = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], NULL, raindeer_routine, &ids[i]);
    }
    pthread_create(&threads[n], NULL, santa_routine, NULL);
    for (int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    return 0;
}

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/stat.h>

#define OVEN_PATH "/cook"
#define TABLE_PATH "/deliverer"

#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define OVEN_SPACE_SEMAPHORE "/OVEN_SPACE_SEMAPHORE" // semaphore for oven space (5 spots)
#define OVEN_SEMAPHORE "/OVEN_SEMAPHORE"             // spot for oven (1 spot)
#define TABLE_SEMAPHORE "/TABLE_SEMAPHORE"           // spot for table  (1 spots)
#define TABLE_FULL "/TABLE_FULL"                     // semaphore for table full (5 avalaible spots, 0 means full)
#define TABLE_EMPTY "/TABLE_EMPTY"                   // semaphore for table empty (5 taken spots, 0 means empty)

// oven structure
typedef struct oven
{
    int pizzas_in_oven[OVEN_SIZE];
    int pizzas_count;
    int last_placed;
    int first_to_be_done;
} oven_t;

// table structure
typedef struct table
{
    int coldest_pizza; // index of the first pizza to take, should be set to upper pizza if taken
    int pizzas_on_table[TABLE_SIZE];
    int pizzas_count;
    int last_placed;
} table_t;

// funtion that returns timestamp in format: hh:mm:ss:mmm using clock_gettime()
char *get_timestamp()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *time_str = malloc(sizeof(char) * 30);
    // date and time in format: YYYY-MM-DD HH:MM:SS
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    char microseconds[20];
    sprintf(microseconds, "%02ld:%03ld", ts.tv_sec % 60, (ts.tv_nsec / 1000000));
    // printf("%s|%s\n", time_str, microseconds);
    strcat(time_str, microseconds);
    return time_str;
}

sem_t *get_semaphore(char *path)
{
    sem_t *semid = sem_open(path, O_RDWR);
    // check for error
    if (semid == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int get_shm_oven_id()
{

    int shmid = shm_open(OVEN_PATH, O_RDWR, 0666);
    // error handling
    if (shmid == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    return shmid;
}
int get_shm_table_id()
{
    int shmid = shm_open(TABLE_PATH, O_RDWR, 0666);
    // error handling
    if (shmid == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// char *semnum_to_str(int semnum)
// {
//     switch (semnum)
//     {
//     case OVEN_SPACE_SEMAPHORE:
//         return "OVEN_SPACE_SEMAPHORE";
//         break;
//     case OVEN_SEMAPHORE:
//         return "OVEN_SEMAPHORE";
//         break;
//     default:
//     case TABLE_SEMAPHORE:
//         return "TABLE_SEMAPHORE";
//         break;
//     case TABLE_FULL:
//         return "TABLE_FULL";
//         break;
//     case TABLE_EMPTY:
//         return "TABLE_EMPTY";
//         break;
//         return "UNKNOWN";
//         break;
//     }
// }

void lock_semaphore(sem_t *semid)
{
    int ret = sem_wait(semid);
    // handle error
    if (ret == -1)
    {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}
void unlock_semaphore(sem_t *semid)
{
    int ret = sem_post(semid);
    // handle error
    if (ret == -1)
    {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

#endif // CONSTANTS_H

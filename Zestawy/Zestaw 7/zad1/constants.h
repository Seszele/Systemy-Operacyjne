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

#define PROJECT_ID 'W'
#define OVEN_SHM_ID 'S'
#define OVEN_PATH "./cook"
#define TABLE_PATH "./deliverer"

#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define OVEN_SPACE_SEMAPHORE 0 // semaphore for oven space (5 spots)
#define OVEN_SEMAPHORE 1       // spot for oven (1 spot)
#define TABLE_SEMAPHORE 2      // spot for table  (1 spots)
#define TABLE_FULL 3           // semaphore for table full (5 avalaible spots, 0 means full)
#define TABLE_EMPTY 4          // semaphore for table empty (5 taken spots, 0 means empty)

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

union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

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

int get_semaphore_id()
{
    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    // printf("key: %d\n", key);
    if (key == -1)
    {
        perror("ftok?");
        exit(1);
    }
    int semid = semget(key, 5, 0);
    if (semid == -1)
    {
        perror("semget - get_semaphore_id");
        exit(1);
    }
    return semid;
}

int get_shm_oven_id()
{
    key_t key = ftok(OVEN_PATH, OVEN_SHM_ID);
    int shmid = shmget(key, sizeof(oven_t), 0);
    // error handling
    if (shmid == -1)
    {
        perror("shmget - get_shm_oven_id");
        exit(1);
    }
    return shmid;
}
int get_shm_table_id()
{
    key_t key = ftok(TABLE_PATH, OVEN_SHM_ID + 1);
    int shmid = shmget(key, sizeof(table_t), 0);
    // error handling
    if (shmid == -1)
    {
        perror("shmget - get_shm_table_id");
        exit(1);
    }
    return shmid;
}

char *semnum_to_str(int semnum)
{
    switch (semnum)
    {
    case OVEN_SPACE_SEMAPHORE:
        return "OVEN_SPACE_SEMAPHORE";
        break;
    case OVEN_SEMAPHORE:
        return "OVEN_SEMAPHORE";
        break;
    default:
    case TABLE_SEMAPHORE:
        return "TABLE_SEMAPHORE";
        break;
    case TABLE_FULL:
        return "TABLE_FULL";
        break;
    case TABLE_EMPTY:
        return "TABLE_EMPTY";
        break;
        return "UNKNOWN";
        break;
    }
}
int get_semaphore_value(int semid, int semnum)
{
    int value = semctl(semid, semnum, GETVAL);
    if (value == -1)
    {
        perror("semctl1 - get_semaphore_value");
        exit(1);
    }
    return value;
}

void lock_semaphore(int semid, int semnum)
{
    struct sembuf sb;
    sb.sem_num = semnum;
    sb.sem_op = -1;
    // sb.sem_flg = 0;
    int errno = semop(semid, &sb, 1);
    // check for errors
    if (errno != 0)
    {
        char *err = strerror(errno);
        printf("%d [%s] Unlock Error: %s\n", getpid(), semnum_to_str(semnum), err);
        exit(1);
    }

    // red color
    // printf("\033[1;31m");
    // printf("[%s] Semaphore locked.%d\n", semnum_to_str(semnum), get_semaphore_value(semid, semnum));
    // printf("\033[0m");
}
void unlock_semaphore(int semid, int semnum)
{
    struct sembuf sb;
    sb.sem_num = semnum;
    sb.sem_op = 1;
    // sb.sem_flg = 0;
    int errno = semop(semid, &sb, 1);
    if (errno != 0)
    {
        char *err = strerror(errno);
        printf("%d [%s] Unlock Error: %s\n", getpid(), semnum_to_str(semnum), err);
        exit(1);
    }
    // green color
    // printf("\033[1;32m");
    // printf("[%s] Semaphore unlocked. %d\n", semnum_to_str(semnum), get_semaphore_value(semid, semnum));
    // printf("\033[0m");
}

#endif // CONSTANTS_H

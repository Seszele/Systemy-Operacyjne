#include "constants.h"

void create_semaphores()
{
    key_t key = ftok(getenv("HOME"), PROJECT_ID);
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }
    // printf("key: %d\n", key);
    int semid = semget(key, 5, 0666 | IPC_CREAT);
    if (semid == -1)
    {
        perror("semget - create_semaphores");
        exit(1);
    }
    // set semaphore values
    union semun arg;
    arg.val = 1;
    if (semctl(semid, OVEN_SEMAPHORE, SETVAL, arg) == -1)
    {
        perror("semctl - create_semaphores");
        exit(1);
    }
    if (semctl(semid, TABLE_SEMAPHORE, SETVAL, arg) == -1)
    {
        perror("semctl - create_semaphores");
        exit(1);
    }

    arg.val = OVEN_SIZE;
    if (semctl(semid, OVEN_SPACE_SEMAPHORE, SETVAL, arg) == -1)
    {
        perror("semctl - create_semaphores");
        exit(1);
    }
    if (semctl(semid, TABLE_FULL, SETVAL, arg) == -1)
    {
        perror("semctl - create_semaphores");
        exit(1);
    }
    arg.val = 0;

    if (semctl(semid, TABLE_EMPTY, SETVAL, arg) == -1)
    {
        perror("semctl - create_semaphores");
        exit(1);
    }

    // printf("Created semaphores, semid: %d\n", semid);
    // printf(" With val:semvalue: %d\n", get_semaphore_value(semid, OVEN_SEMAPHORE));
}
void oven_init(oven_t *oven)
{
    // check if oven exists
    if (oven == NULL)
    {
        printf("Oven is NULL\n");
        exit(1);
    }
    // set pizzas to -1
    for (int i = 0; i < OVEN_SIZE; i++)
    {
        oven->pizzas_in_oven[i] = -1;
    }
    oven->first_to_be_done = 0;
    oven->last_placed = -1;
    oven->pizzas_count = 0;
}
void table_init(table_t *table)
{
    // check if table exists
    if (table == NULL)
    {
        printf("Table is NULL\n");
        exit(1);
    }
    // set pizzas to -1
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table->pizzas_on_table[i] = -1;
    }
    table->coldest_pizza = 0;
    table->last_placed = -1;
    table->pizzas_count = 0;
}
void create_shm()
{
    key_t o_key = ftok(OVEN_PATH, OVEN_SHM_ID);
    if (o_key == -1)
    {
        perror("ftok_o");
        exit(1);
    }
    key_t t_key = ftok(TABLE_PATH, OVEN_SHM_ID + 1);
    if (t_key == -1)
    {
        perror("ftok_t");
        exit(1);
    }
    int o_shmid = shmget(o_key, sizeof(oven_t), 0666 | IPC_CREAT);
    if (o_shmid == -1)
    {
        perror("shmget - create_shm_o");
        exit(1);
    }
    int t_shmid = shmget(t_key, sizeof(table_t), 0666 | IPC_CREAT);
    if (t_shmid == -1)
    {
        perror("shmget - create_shm_t");
        exit(1);
    }
    oven_init((oven_t *)shmat(o_shmid, NULL, 0));
    table_init((table_t *)shmat(t_shmid, NULL, 0));
}

int main(int argc, char const *argv[])
{
    create_shm();
    create_semaphores();
    // check if 2 agruments are given
    if (argc != 3)
    {
        printf("usage: ./main <number of cooks> <number of deliverers>\n");
        exit(1);
    }
    int cooks = atoi(argv[1]);
    int deliverers = atoi(argv[2]);
    // red
    printf("\033[1;31m================================================\n");
    printf("Beginning program with %d cooks and %d deliverers=\n", cooks, deliverers);
    printf("================================================\033[0m\n");
    for (int i = 0; i < cooks; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            execl("./cook", "./cook", NULL);
        }
    }
    for (int i = 0; i < deliverers; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            execl("./deliverer", "./deliverer", NULL);
        }
    }

    for (int i = 0; i < cooks + deliverers; i++)
        wait(NULL);
    return 0;
}

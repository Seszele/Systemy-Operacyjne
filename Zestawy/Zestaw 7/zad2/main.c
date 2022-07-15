#include "constants.h"

void create_semaphores()
{
    sem_open(OVEN_SPACE_SEMAPHORE, O_CREAT, 0666, OVEN_SIZE);
    sem_open(OVEN_SEMAPHORE, O_CREAT, 0666, 1);
    sem_open(TABLE_SEMAPHORE, O_CREAT, 0666, 1);
    sem_open(TABLE_FULL, O_CREAT, 0666, TABLE_SIZE);
    sem_open(TABLE_EMPTY, O_CREAT, 0666, 0);
    // check errors
    if (errno)
    {
        // perror("sem_open - create");
        exit(EXIT_FAILURE);
    }
}
void handler(int sig)
{
    printf("\033[31m%d - %s\033[0m Wychodze.\n", getpid(), get_timestamp());
    sem_unlink(OVEN_SPACE_SEMAPHORE);
    sem_unlink(OVEN_SEMAPHORE);
    sem_unlink(TABLE_SEMAPHORE);
    sem_unlink(TABLE_FULL);
    sem_unlink(TABLE_EMPTY);
    exit(EXIT_SUCCESS);
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
    // create shared memory
    int shm_o = shm_open(OVEN_PATH, O_CREAT | O_RDWR, 0666);
    int shm_t = shm_open(TABLE_PATH, O_CREAT | O_RDWR, 0666);
    // error handling
    if (shm_o == -1 || shm_t == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    // set size of shared memory
    if (ftruncate(shm_o, sizeof(oven_t)) == -1 || ftruncate(shm_t, sizeof(table_t)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    // map shared memory
    oven_t *oven = (oven_t *)mmap(NULL, sizeof(oven_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_o, 0);
    table_t *table = (table_t *)mmap(NULL, sizeof(table_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_t, 0);
    // error handling
    if (oven == MAP_FAILED || table == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    oven_init(oven);
    table_init(table);
}

int main(int argc, char const *argv[])
{

    // check if 2 agruments are given
    int cooks = atoi(argv[1]);
    int deliverers = atoi(argv[2]);
    if (argc == 3)
    {
        char buf[15];
        sprintf(buf, "./main %d %d c", cooks, deliverers);
        system(buf);
    }

    create_semaphores();

    // handle sigint
    signal(SIGINT, handler);
    create_shm();
    // red
    printf("\033[1;31m===============================================\n");
    printf("Beginning program with %d cooks and %d deliverers\n", cooks, deliverers);
    printf("===============================================\033[0m\n");
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

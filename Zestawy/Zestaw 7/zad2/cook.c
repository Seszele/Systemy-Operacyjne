// import librariers <sys/shm.h> <sys/ipc.h>
#include "constants.h"

void add_to_oven(oven_t *oven, int pizza_type)
{
    // add cyclic
    oven->last_placed = (oven->last_placed + 1) >= OVEN_SIZE ? 0 : oven->last_placed + 1;
    if (oven->pizzas_count == 0)
        oven->first_to_be_done = oven->last_placed;
    oven->pizzas_in_oven[oven->last_placed] = pizza_type;
    oven->pizzas_count++;
    // return oven->last_placed;
}
int remove_from_oven(oven_t *oven)
{
    int pizza_type = oven->pizzas_in_oven[oven->first_to_be_done];
    oven->pizzas_in_oven[oven->first_to_be_done] = -1;
    oven->first_to_be_done = (oven->first_to_be_done + 1) >= OVEN_SIZE ? 0 : oven->first_to_be_done + 1;
    oven->pizzas_count--;
    return pizza_type;
}
void place_on_the_table(table_t *table, int pizza_type)
{
    table->last_placed = (table->last_placed + 1) >= TABLE_SIZE ? 0 : table->last_placed + 1;
    if (table->pizzas_count == 0)
        table->coldest_pizza = table->last_placed;

    table->pizzas_on_table[table->last_placed] = pizza_type;
    table->pizzas_count++;
}

void cook(sem_t *oven_space_semaphore, sem_t *oven_semaphore, sem_t *table_full, sem_t *table_semaphore, sem_t *table_empty, oven_t *oven, table_t *table)
{
    // get random number between 0 and 9
    int pizza_type = rand() % 10;
    sleep(rand() % 2 + 1); // sleep for random time between 1 and 2 seconds

    printf("\033[33m%d - %s\033[0m Przygotowuje pizze: %d.\n", getpid(), get_timestamp(), pizza_type);

    //  add to oven
    // wait for place in oven
    lock_semaphore(oven_space_semaphore);
    // wait for oven to be free
    lock_semaphore(oven_semaphore);
    add_to_oven(oven, pizza_type);
    printf("\033[33m%d - %s\033[0m Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_timestamp(), pizza_type, oven->pizzas_count);
    unlock_semaphore(oven_semaphore);

    sleep(rand() % 2 + 4); // sleep for random time between 4 and 5 seconds
    fflush(stdout);
    // take pizza from oven
    lock_semaphore(oven_semaphore);
    pizza_type = remove_from_oven(oven);
    unlock_semaphore(oven_semaphore);
    unlock_semaphore(oven_space_semaphore);
    fflush(stdout);
    // lock table
    lock_semaphore(table_full);
    lock_semaphore(table_semaphore);
    //  place on the table
    place_on_the_table(table, pizza_type);
    unlock_semaphore(table_empty);
    unlock_semaphore(table_semaphore);
    // unlock table
    printf("\033[33m%d - %s\033[0m Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole:%d.\n", getpid(), get_timestamp(), pizza_type, oven->pizzas_count, table->pizzas_count);
}

int main(int argc, char const *argv[])
{
    srand(getpid());

    // int semid = get_semaphore_id();
    sem_t *oven_space_semaphore = get_semaphore(OVEN_SPACE_SEMAPHORE);
    sem_t *oven_semaphore = get_semaphore(OVEN_SEMAPHORE);
    sem_t *table_full = get_semaphore(TABLE_FULL);
    sem_t *table_semaphore = get_semaphore(TABLE_SEMAPHORE);
    sem_t *table_empty = get_semaphore(TABLE_EMPTY);
    // get oven from shared memory
    int o_shmid = get_shm_oven_id();
    int t_shmid = get_shm_table_id();
    // check erorr
    if (o_shmid == -1)
    {
        perror("Error while getting shared memory id");
        return 1;
    }
    if (t_shmid == -1)
    {
        perror("Error while getting shared memory id");
        return 1;
    }
    oven_t *oven = mmap(NULL, sizeof(oven_t), PROT_READ | PROT_WRITE, MAP_SHARED, o_shmid, 0);
    table_t *table = mmap(NULL, sizeof(table_t), PROT_READ | PROT_WRITE, MAP_SHARED, t_shmid, 0);
    // check error
    if (oven == (void *)-1)
    {
        perror("Error while attaching shared memory");
        return 1;
    }
    if (table == (void *)-1)
    {
        perror("Error while attaching shared memory");
        return 1;
    }

    while (1)
    {
        cook(oven_space_semaphore, oven_semaphore, table_full, table_semaphore, table_empty, oven, table);
    }
    return 0;
}

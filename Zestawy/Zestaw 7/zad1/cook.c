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

void cook(int semid, oven_t *oven, table_t *table)
{
    // get random number between 0 and 9
    int pizza_type = rand() % 10;
    sleep(rand() % 2 + 1); // sleep for random time between 1 and 2 seconds

    printf("\033[33m%d - %s\033[0m Przygotowuje pizze: %d.\n", getpid(), get_timestamp(), pizza_type);

    //  add to oven
    // wait for place in oven
    lock_semaphore(semid, OVEN_SPACE_SEMAPHORE);
    // wait for oven to be free
    lock_semaphore(semid, OVEN_SEMAPHORE);
    add_to_oven(oven, pizza_type);
    printf("\033[33m%d - %s\033[0m Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_timestamp(), pizza_type, oven->pizzas_count);
    unlock_semaphore(semid, OVEN_SEMAPHORE);

    sleep(rand() % 2 + 4); // sleep for random time between 4 and 5 seconds

    // take pizza from oven
    lock_semaphore(semid, OVEN_SEMAPHORE);
    pizza_type = remove_from_oven(oven);
    unlock_semaphore(semid, OVEN_SPACE_SEMAPHORE);
    unlock_semaphore(semid, OVEN_SEMAPHORE);

    // lock table
    lock_semaphore(semid, TABLE_FULL);
    lock_semaphore(semid, TABLE_SEMAPHORE);
    //  place on the table
    place_on_the_table(table, pizza_type);
    unlock_semaphore(semid, TABLE_EMPTY);
    unlock_semaphore(semid, TABLE_SEMAPHORE);
    // unlock table
    printf("\033[33m%d - %s\033[0m Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole:%d.\n", getpid(), get_timestamp(), pizza_type, oven->pizzas_count, table->pizzas_count);
}

int main(int argc, char const *argv[])
{
    srand(getpid());

    int semid = get_semaphore_id();
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
    oven_t *oven = shmat(o_shmid, NULL, 0);
    table_t *table = shmat(t_shmid, NULL, 0);
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
        cook(semid, oven, table);
    }
    return 0;
}

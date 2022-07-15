#include "constants.h"

int take_pizza(table_t *table)
{
    int pizza_type = table->pizzas_on_table[table->coldest_pizza];
    table->pizzas_on_table[table->coldest_pizza] = -1;
    table->coldest_pizza = (table->coldest_pizza + 1) >= TABLE_SIZE ? 0 : table->coldest_pizza + 1;
    table->pizzas_count--;
    return pizza_type;
}

void deliver(int semid, table_t *table)
{
    lock_semaphore(semid, TABLE_EMPTY);
    lock_semaphore(semid, TABLE_SEMAPHORE);
    int pizza_type = take_pizza(table);
    // Wypisanie komunikatu: (pid timestamp) Pobieram pizze: n Liczba pizz na stole: k.
    printf("\033[34m%d - %s\033[0m Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_timestamp(), pizza_type, table->pizzas_count);
    unlock_semaphore(semid, TABLE_SEMAPHORE);
    unlock_semaphore(semid, TABLE_FULL);

    // wait 4-5s
    sleep(rand() % 2 + 4);

    // blue color

    // Wypisanie komunikatu: (pid timestamp) Dostarczam pizze: n.
    printf("\033[34m%d - %s\033[0m Dostarczam pizze: %d.\n", getpid(), get_timestamp(), pizza_type);

    sleep(rand() % 2 + 4);
}

int main(int argc, char const *argv[])
{
    int semid = get_semaphore_id();
    int t_shmid = get_shm_table_id();
    if (t_shmid == -1)
    {
        perror("Error while getting shared memory id");
        return 1;
    }
    table_t *table = shmat(t_shmid, NULL, 0);
    if (table == (void *)-1)
    {
        perror("Error while attaching shared memory");
        return 1;
    }
    while (1)
    {
        deliver(semid, table);
    }

    return 0;
}
// remember to set coldest pizza on takeout

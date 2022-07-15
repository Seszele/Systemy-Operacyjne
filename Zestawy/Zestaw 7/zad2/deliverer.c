#include "constants.h"

sem_t *oven_space_semaphore;
sem_t *oven_semaphore;
sem_t *table_full;
sem_t *table_semaphore;
sem_t *table_empty;
int take_pizza(table_t *table)
{
    int pizza_type = table->pizzas_on_table[table->coldest_pizza];
    table->pizzas_on_table[table->coldest_pizza] = -1;
    table->coldest_pizza = (table->coldest_pizza + 1) >= TABLE_SIZE ? 0 : table->coldest_pizza + 1;
    table->pizzas_count--;
    return pizza_type;
}

void deliver(table_t *table)
{
    lock_semaphore(table_empty);
    lock_semaphore(table_semaphore);
    int pizza_type = take_pizza(table);
    // Wypisanie komunikatu: (pid timestamp) Pobieram pizze: n Liczba pizz na stole: k.
    printf("\033[34m%d - %s\033[0m Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_timestamp(), pizza_type, table->pizzas_count);
    unlock_semaphore(table_semaphore);
    unlock_semaphore(table_full);

    // wait 4-5s
    sleep(rand() % 2 + 4);

    // blue color

    // Wypisanie komunikatu: (pid timestamp) Dostarczam pizze: n.
    printf("\033[34m%d - %s\033[0m Dostarczam pizze: %d.\n", getpid(), get_timestamp(), pizza_type);

    sleep(rand() % 2 + 4);
}

int main(int argc, char const *argv[])
{

    oven_space_semaphore = get_semaphore(OVEN_SPACE_SEMAPHORE);
    oven_semaphore = get_semaphore(OVEN_SEMAPHORE);
    table_full = get_semaphore(TABLE_FULL);
    table_semaphore = get_semaphore(TABLE_SEMAPHORE);
    table_empty = get_semaphore(TABLE_EMPTY);
    int t_shmid = get_shm_table_id();
    if (t_shmid == -1)
    {
        perror("Error while getting shared memory id");
        return 1;
    }
    table_t *table = mmap(NULL, sizeof(table_t), PROT_READ | PROT_WRITE, MAP_SHARED, t_shmid, 0);
    if (table == (void *)-1)
    {
        perror("Error while attaching shared memory");
        return 1;
    }
    while (1)
    {
        deliver(table);
    }

    return 0;
}
// remember to set coldest pizza on takeout

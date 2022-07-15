#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

FILE *file;
int width;
int height;
int max_value; // 255 max
int **image;
int **negated;
int thread_count;

int read_image_file(const char *filename)
{
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", filename);
        return -1;
    }
    char line[256];
    fscanf(file, "%s", line);
    if (strcmp(line, "P2") != 0)
    {
        printf("Wrong file format\n");
        return -1;
    }
    // skip next line
    fgets(line, 255, file);
    fgets(line, 255, file);
    // read width and height
    fscanf(file, "%d %d", &width, &height);
    fgets(line, 255, file);
    fscanf(file, "%d", &max_value);

    image = (int **)malloc(height * sizeof(int *));
    // check error
    if (image == NULL)
    {
        printf("Could not allocate memory for image\n");
        return -1;
    }
    negated = (int **)malloc(height * sizeof(int *));
    // check error
    if (negated == NULL)
    {
        printf("Could not allocate memory for negated\n");
        return -1;
    }
    for (int i = 0; i < height; i++)
    {
        image[i] = (int *)malloc(width * sizeof(int));
        // check error
        if (image[i] == NULL)
        {
            printf("Could not allocate memory for image_\n");
            return -1;
        }
        negated[i] = (int *)malloc(width * sizeof(int));
        // check error
        if (negated[i] == NULL)
        {
            printf("Could not allocate memory for negated_\n");
            return -1;
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fscanf(file, "%d", &image[i][j]); // image[y][x]
            negated[i][j] = -1;
        }
    }
    fclose(file);
    return 0;
}

void negate_val(void *arg)
{
    // negate image but only in the thread's value range
    int start = max_value * (int)arg / (thread_count);
    int end = max_value * ((int)arg + 1) / (thread_count);
    printf("Thread %d started: %d - %d\n", (int)arg, start, end);
    // timer
    time_t timer = time(NULL);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (negated[i][j] != -1)
                continue;
            if ((image[i][j] >= start && image[i][j] < end) || (image[i][j] == max_value && end == max_value))
            {
                // image[i][j] = max_value - image[i][j];
                negated[i][j] = max_value - image[i][j];
            }
        }
    }
    timer = time(NULL) - timer;
    pthread_exit((void *)timer);
}
void negate_block(void *arg)
{
    // negate image but only in the thread's value range
    int start = width * (int)arg / (thread_count);
    int end = width * ((int)arg + 1) / (thread_count);
    // printf("Thread %d started: %d - %d\n", (int)arg, start, end);
    // timer
    time_t timer = time(NULL);
    for (int i = 0; i < height; i++)
    {
        for (int j = start; j < end; j++)
        {
            if (negated[i][j] == -1)
            {
                negated[i][j] = max_value - image[i][j];
                // if (i == 5 && j == 5)
                // {
                //     printf("%d!!!!!\n", image[i][j]);
                // }
                // negated[i][j] = 1;
            }
        }
    }
    timer = time(NULL) - timer;
    pthread_exit((void *)timer);
}
void save_image(const char output_filename[])
{
    file = fopen(output_filename, "w");
    if (file == NULL)
    {
        printf("Could not open file %s\n", output_filename);
        return;
    }
    fprintf(file, "P2\n");
    fprintf(file, "# Created by zad1\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max_value);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fprintf(file, "%d ", negated[i][j]);
        }
        fprintf(file, "\n");
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        printf("Usage: <thread_count> <type> <image_file> <output_file>\n");
        return -1;
    }
    if (read_image_file(argv[3]) != 0)
    {
        return -1;
    }
    thread_count = atoi(argv[1]);
    pthread_t threads[thread_count];
    void *func = NULL;
    if (strcmp(argv[2], "block") == 0)
    {
        func = negate_block;
    }
    else if (strcmp(argv[2], "numbers") == 0)
    {
        func = negate_val;
    }
    else
    {
        printf("Wrong type\n");
        return -1;
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, func, (void *)i);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    save_image(argv[4]);
    return 0;
}

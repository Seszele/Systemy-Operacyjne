#ifndef DYNAMIC
#include "mylib.h"
#endif
#ifdef DYNAMIC
#include <dlfcn.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>

long double inSeconds(clock_t start, clock_t end)
{
    return ((long double)(end - start) / sysconf(_SC_CLK_TCK));
}
void saveToFile(clock_t time1, clock_t time2, struct tms *tms1, struct tms *tms2, char *testDescription)
{
    time2 = times(tms2);
    printf("%s\n", testDescription);
    printf("\trealtime: %Lf\n", inSeconds(time1, time2));
    printf("\tusertime: %Lf\n", inSeconds(tms1->tms_utime, tms2->tms_utime));
    printf("\tsystemtime: %Lf\n", inSeconds(tms1->tms_stime, tms2->tms_stime));

    FILE *raport2 = fopen("./raport3b.txt", "a");

    fprintf(raport2, "%s\n", testDescription);
    fprintf(raport2, "\trealtime: %Lf\n", inSeconds(time1, time2));
    fprintf(raport2, "\tusertime: %Lf\n", inSeconds(tms1->tms_utime, tms2->tms_utime));
    fprintf(raport2, "\tsystemtime: %Lf\n", inSeconds(tms1->tms_stime, tms2->tms_stime));

    fclose(raport2);
}

int main(int argc, char *argv[])
{
#ifdef DYNAMIC
    void *handle = dlopen("./libmylib.so", RTLD_LAZY);
    if (!handle)
    { /*error*/
        puts("Bad handle ERR");
    }

    void **(*reserveArray)(int);
    reserveArray = (void **(*)(int))dlsym(handle, "reserveArray");
    int (*getFileInfo)(char *);
    getFileInfo = (int (*)(char *))dlsym(handle, "getFileInfo");
    int (*saveBlock)(int, void **, int);
    saveBlock = (int (*)(int, void **, int))dlsym(handle, "saveBlock");
    int (*removeBlock)(int, void **, int);
    removeBlock = (int (*)(int, void **, int))dlsym(handle, "removeBlock");

    if (dlerror() != NULL)
    { /*error*/
        puts("dlerror ERR");
    }
#endif

    struct tms *tms1 = calloc(1, sizeof(struct tms *));
    struct tms *tms2 = calloc(1, sizeof(struct tms *));
    clock_t time1 = 0;
    clock_t time2 = 0;

    if ((argc - 1) % 2 != 0)
    {
        puts("Arguments dont match the number of tasks! ERR");
        return 0;
    }
    if (argc == 1)
    {
        puts("No arguments! ERR");
        return 0;
    }

    void **array = 0;
    int arrSize = 0;
    char *testDescription;
    // t-tablica  z-zlicz  u-usun s-miejsce od ktorego zapisac czas wykonania
    for (int i = 1; i < argc; i += 2)
    {
        switch (argv[i][0])
        {
        case 't':
        {
            arrSize = atoi(argv[i + 1]);
            array = reserveArray(arrSize);
            break;
        }
        case 'z':
        {
            int fd = getFileInfo(argv[i + 1]);
            if (saveBlock(fd, array, arrSize) == -1)
            {
                printf("%s", "Not enaugh space for: ");
                printf("%s\n", argv[i + 1]);
            }
            break;
        }
        case 'u':
        {
            if (removeBlock(atoi(argv[i + 1]), array, arrSize) == -1)
            {
                printf("%s", "Cannot remove block from index: ");
                printf("%s\n", argv[i + 1]);
            }
            break;
        }
        case 's':
        {
            if (time1 != 0)
            {
                // already meassuring time, save previous
                saveToFile(time1, time2, tms1, tms2, testDescription);
            }

            time1 = times(tms1);
            testDescription = argv[i + 1];

            break;
        }

        default:
            printf("%s", "Invalid task: ");
            printf("%c\n", argv[i][0]);
            break;
        }
    }
    if (time1 != 0)
    {
        saveToFile(time1, time2, tms1, tms2, testDescription);
    }
#ifdef DYNAMIC
    dlclose(handle);
#endif
    return 0;
}
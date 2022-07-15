#include "mylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void **reserveArray(int size)
{
    void **newArray = calloc(size, sizeof(void *));
    return newArray;
}
int getFileInfo(char *fileName)
{
    int fd;
    char tmpName[] = "/tmp/fileXXXXXX";
    fd = mkstemp(tmpName);
    char command[60] = "wc -l -w -m ";
    strcat(command, fileName);
    strcat(command, " > ");
    strcat(command, tmpName);
    if (system(command) == -1)
    {
        return 0;
    }
    return fd;
}
int saveBlock(int fd, void **array, int size) // returns array index, -1 if array is full
{
    int index = -1;
    for (int i = 0; i < size; i++)
    {
        if (array[i] == 0)
        {
            FILE *f = fdopen(fd, "w+");
            fseek(f, 0, SEEK_END);
            int size = ftell(f);
            fseek(f, 0, SEEK_SET);
            array[i] = calloc(1, size);

            if (fread(array[i], size, 1, f) == 0)
            {
                return -1;
            }
            fclose(f);

            index = i;
            break;
        }
    }

    return index;
}
int removeBlock(int index, void **array, int size) // removes block, returns 1 if succeds and -1 otherwise
{
    if (index >= size || array[index] == 0)
    {
        return -1;
    }
    free(array[index]);
    array[index] = 0;
    return 1;
}
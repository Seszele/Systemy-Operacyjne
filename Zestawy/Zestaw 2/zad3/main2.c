#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/limits.h>

// #include <unistd.h>

struct FileTypeCounter
{
    int file;
    int dir;
    int char_dev;
    int block_dev;
    int fifo;
    int slink;
    int sock;
};
struct FileTypeCounter typeCounter;
int skippedParent = -1;
int determineType(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt)
{
    // skip the parent directory
    if (skippedParent == -1)
    {
        skippedParent = 1;
        return 0;
    }

    char absolutePath[PATH_MAX];
    char exactName[PATH_MAX];
    char tempPath[PATH_MAX];

    // get the name and cut it out of the path
    strcpy(tempPath, filename);
    for (int i = strlen(tempPath) - 1; i >= 0; i--)
    {
        if (tempPath[i] == '/')
        {
            for (int j = i + 1; j < strlen(tempPath); j++)
            {
                exactName[j - i - 1] = tempPath[j];
                exactName[j - i] = '\0';
            }
            tempPath[i + 1] = '\0';
            break;
        }
    }

    if (realpath(tempPath, absolutePath) == NULL)
    {
        printf("\tCould not get an absolute path\n");
        return 0;
    }
    strcat(absolutePath, "/");
    strcat(absolutePath, exactName);

    printf("%s:\n", exactName);
    printf("\t%s\n", absolutePath);

    mode_t mode = statptr->st_mode;
    switch (mode & S_IFMT)
    {
    case S_IFLNK:
        printf("\tLink symboliczny\n");
        typeCounter.slink++;
        break;
    case S_IFDIR:
        printf("\tKatalog\n");
        typeCounter.dir++;
        break;
    case S_IFREG:
        printf("\tZwykly plik\n");
        typeCounter.file++;
        break;
    case S_IFCHR:
        printf("\tUrzadzenie znakowe\n");
        typeCounter.char_dev++;
        break;
    case S_IFBLK:
        printf("\tUrzadzenie blokowe\n");
        typeCounter.block_dev++;
        break;
    case S_IFIFO:
        printf("\tPotok nazwany\n");
        typeCounter.fifo++;
        break;
    case S_IFSOCK:
        printf("\tSoket\n");
        typeCounter.sock++;
        break;
    default:
        printf("\tCould not recognize the file type\n");
        break;
    }

    printf("\t%ld\n", statptr->st_size);
    printf("\t%s", ctime(&statptr->st_atime));
    printf("\t%s", ctime(&statptr->st_atime));

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        puts("1 argument needed! ERR");
        return 0;
    }

    typeCounter.file = 0;
    typeCounter.dir = 0;
    typeCounter.char_dev = 0;
    typeCounter.block_dev = 0;
    typeCounter.fifo = 0;
    typeCounter.slink = 0;
    typeCounter.sock = 0;

    nftw(argv[1], determineType, 20, FTW_PHYS);

    printf("==========================\n");
    printf("Zwykle pliki: %d\n", typeCounter.file);
    printf("Katalogi: %d\n", typeCounter.dir);
    printf("Pliki specjalne znakowe: %d\n", typeCounter.char_dev);
    printf("Pliki specjalne blokowe: %d\n", typeCounter.block_dev);
    printf("Potoki/kolejki FIFO: %d\n", typeCounter.fifo);
    printf("Linki symboliczne: %d\n", typeCounter.slink);
    printf("Sokety: %d\n", typeCounter.sock);
    printf("==========================\n");

    return 0;
}
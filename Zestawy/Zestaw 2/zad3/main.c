#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

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

void determineType(mode_t mode, struct FileTypeCounter *typeCounter)
{
    switch (mode & S_IFMT)
    {
    case S_IFLNK:
        printf("\tLink symboliczny\n");
        typeCounter->slink++;
        break;
    case S_IFDIR:
        printf("\tKatalog\n");
        typeCounter->dir++;
        break;
    case S_IFREG:
        printf("\tZwykly plik\n");
        typeCounter->file++;
        break;
    case S_IFCHR:
        printf("\tUrzadzenie znakowe\n");
        typeCounter->char_dev++;
        break;
    case S_IFBLK:
        printf("\tUrzadzenie blokowe\n");
        typeCounter->block_dev++;
        break;
    case S_IFIFO:
        printf("\tPotok nazwany\n");
        typeCounter->fifo++;
        break;
    case S_IFSOCK:
        printf("\tSoket\n");
        typeCounter->sock++;
        break;
    default:
        printf("\tCould not recognize the file type\n");
        break;
    }
}

void traverseDir(char *path, struct FileTypeCounter *typeCounter)
{
    DIR *dir;
    if ((dir = opendir(path)) == NULL)
    {
        puts("No such directory! ERR");
        return;
    }
    struct dirent *file;

    while ((file = readdir(dir)) != NULL)
    {
        if (file->d_name[0] == '.')
        {
            continue;
        }
        char appendedPath[PATH_MAX];
        snprintf(appendedPath, sizeof(appendedPath), "%s/%s", path, file->d_name);
        char absolutePath[PATH_MAX];

        struct stat stats;
        lstat(appendedPath, &stats);

        printf("%s:\n", file->d_name);
        if (realpath(path, absolutePath) == NULL)
        {
            printf("\tCould not get an absolute path\n");
        }
        else
        {
            strcat(absolutePath, "/");
            strcat(absolutePath, file->d_name);
            printf("\t%s\n", absolutePath);
        }
        printf("\t%ld\n", stats.st_nlink);
        determineType(stats.st_mode, typeCounter);
        printf("\t%ld\n", stats.st_size);
        printf("\t%s", ctime(&stats.st_atim.tv_sec));
        printf("\t%s", ctime(&stats.st_atim.tv_sec));

        if ((stats.st_mode & S_IFMT) == S_IFDIR)
            traverseDir(appendedPath, typeCounter);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        puts("1 argument needed! ERR");
        return 0;
    }

    struct FileTypeCounter typeCounter;
    typeCounter.file = 0;
    typeCounter.dir = 0;
    typeCounter.char_dev = 0;
    typeCounter.block_dev = 0;
    typeCounter.fifo = 0;
    typeCounter.slink = 0;
    typeCounter.sock = 0;

    traverseDir(argv[1], &typeCounter);

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
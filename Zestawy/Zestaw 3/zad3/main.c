#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// https://stackoverflow.com/questions/32571182/check-if-input-is-a-txt-file-in-c
int isTxt(char *fileName) // returns 1 if is a txt file   0 otherwise
{
    char buf[64] = {0};
    char cmd[64] = {0};
    FILE *pp = NULL;

    snprintf(cmd, 64, "%s %s", "file", fileName);

    if ((pp = popen(cmd, "r")))
    {
        if (fgets(buf, 64, pp))
        {
            pclose(pp);

            if (strstr(buf, "ASCII"))
            {
                return 1;
            }
            else
                return 0;
        }
    }
    return 0;
}

void traverseDir(char *path, char *substring, int depthLeft)
{
    if (depthLeft < 0)
    {
        return;
    }

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

        struct stat stats;
        lstat(appendedPath, &stats);

        if (isTxt(appendedPath))
        {
            // check if has str in it
            FILE *f = fopen(appendedPath, "r");
            if (f == NULL)
            {
                puts("Could not open the file! ERR");
                return;
            }
            fseek(f, 0, SEEK_END);
            int src_size = ftell(f);
            fseek(f, 0, SEEK_SET);

            char *read_buffer = calloc(src_size, 1);

            if (fread(read_buffer, 1, src_size, f) != src_size)
            {
                puts("Cannot read the file! ERR");
                return;
            }
            if (strstr(read_buffer, substring) != NULL)
            {
                printf("%s %d %s\n", appendedPath, getpid(), file->d_name);
            }
        }

        if ((stats.st_mode & S_IFMT) == S_IFDIR)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                traverseDir(appendedPath, substring, depthLeft - 1);
                exit(0);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        puts("Potrzebne 3 argumenty! ERR");
        return 0;
    }
    traverseDir(argv[1], argv[2], atoi(argv[3]) - 1);

    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void getInputFiles(char **src, char **dest)
{
    *src = calloc(50, 1);
    *dest = calloc(50, 1);
    int scanned_src = 0;
    int scanned_dest = 0;
    do
    {
        printf("source>> ");
        scanned_src = scanf("%s", *src);
        printf("destination>> ");
        scanned_dest = scanf(" %s", *dest);
    } while (scanned_src != 1 || scanned_dest != 1);
}

char *pruneBlankLines(char *buffer, int size)
{
    char *new_buf = calloc(size, 1);
    int last_i = 0;
    int is_blank = 1;
    int prev_head = 0;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\t' && buffer[i] != '\r' && buffer[i] != '\0')
        {
            is_blank = 0;
        }

        if (buffer[i] == '\n' || i == size - 1)
        {
            // check if isnt empty, if so add this line (from lasti to i) into new_buf
            if (is_blank == 0)
            {
                for (int j = 0; j < i - last_i + 1; j++)
                {
                    new_buf[prev_head + j] = buffer[last_i + j];
                }
                prev_head = prev_head + (i - last_i) + 1;
                is_blank = 1;
            }
            last_i = i + 1;
        }
    }
    free(buffer);
    new_buf[prev_head] = '\0';
    return new_buf;
}

int main(int argc, char *argv[])
{
    char *src = argv[1];
    char *dest = argv[2];
    if (argc > 3)
    {
        puts("Too many arguments (required 2) ERR");
        getInputFiles(&src, &dest);
    }
    if (argc < 3)
    {
        puts("2 arguments required! ERR");
        getInputFiles(&src, &dest);
    }
#ifdef SYS
    int fd_src = open(src, O_RDONLY);
    if (fd_src < 0)
    {
        puts("Could not open the source file! ERR");
        return 0;
    }
    // O_APPEND or O_TRUNC?
    int fd_dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_dest < 0)
    {
        puts("Could not open or create the destination file! ERR");
        return 0;
    }

    off_t src_size;
    src_size = lseek(fd_src, 0, SEEK_END);
    lseek(fd_src, 0, SEEK_SET);

    char *read_buffer = calloc(src_size, 1);

    if (read(fd_src, read_buffer, src_size) != src_size)
    {
        puts("Cannot read the source file! ERR");
        return 0;
    }
    //
    read_buffer = pruneBlankLines(read_buffer, src_size);

    if (write(fd_dest, read_buffer, src_size) != src_size)
    {
        puts("Cannot write to the destination file! ERR");
        return 0;
    }
    // puts("Version: sys");
#endif
#ifdef LIB
    FILE *f_src = fopen(src, "r");
    if (f_src == NULL)
    {
        puts("Could not open the source file! ERR");
        return 0;
    }
    FILE *f_dest = fopen(dest, "w");
    if (f_dest == NULL)
    {
        puts("Could not open or create the destination file! ERR");
        return 0;
    }
    fseek(f_src, 0, SEEK_END);
    int src_size = ftell(f_src);
    fseek(f_src, 0, SEEK_SET);

    char *read_buffer = calloc(src_size, 1);

    if (fread(read_buffer, 1, src_size, f_src) != src_size)
    {
        puts("Cannot read the source file! ERR");
        return 0;
    }
    read_buffer = pruneBlankLines(read_buffer, src_size);
    if (fwrite(read_buffer, 1, src_size, f_dest) != src_size)
    {
        puts("Cannot write to the destination file! ERR");
        return 0;
    }
    // puts("Version: lib");
#endif
    return 0;
}
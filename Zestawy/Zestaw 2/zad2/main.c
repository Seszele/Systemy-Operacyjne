#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void countOccurences(char *buffer, char c, int size, int *res_count, int *res_lines)
{
    int char_count = 0;
    int lines = 0;
    int char_in_line = 0;
    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == c)
        {
            char_in_line++;
        }

        if (buffer[i] == '\n' || i == size - 1)
        {
            // check if line has the char, if so count the line
            if (char_in_line > 0)
            {
                lines++;
            }
            char_count += char_in_line;
            char_in_line = 0;
        }
    }
    *res_count = char_count;
    *res_lines = lines;
    // printf("c:%d, l:%d\n", char_count, lines);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        puts("2 arguments required! ERR");
        return 0;
    }
    char c = argv[1][0];
    char *src = argv[2];

#ifdef SYS
    int fd_src = open(src, O_RDONLY);
    if (fd_src < 0)
    {
        puts("Could not open the file! ERR");
        return 0;
    }

    off_t src_size;
    src_size = lseek(fd_src, 0, SEEK_END);
    lseek(fd_src, 0, SEEK_SET);

    char *read_buffer = calloc(src_size, 1);

    if (read(fd_src, read_buffer, src_size) != src_size)
    {
        puts("Cannot read the file! ERR");
        return 0;
    }
    //
    int count = 0;
    int lines = 0;
    countOccurences(read_buffer, c, src_size, &count, &lines);
    printf("c:%d, l:%d\n", count, lines);
#endif
#ifdef LIB
    FILE *f_src = fopen(src, "r");
    if (f_src == NULL)
    {
        puts("Could not open the file! ERR");
        return 0;
    }
    fseek(f_src, 0, SEEK_END);
    int src_size = ftell(f_src);
    fseek(f_src, 0, SEEK_SET);

    char *read_buffer = calloc(src_size, 1);

    if (fread(read_buffer, 1, src_size, f_src) != src_size)
    {
        puts("Cannot read the file! ERR");
        return 0;
    }
    int count = 0;
    int lines = 0;
    countOccurences(read_buffer, c, src_size, &count, &lines);
    printf("c:%d, l:%d\n", count, lines);
#endif

    return 0;
}
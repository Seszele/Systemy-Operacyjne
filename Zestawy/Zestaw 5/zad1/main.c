#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 10

int read_file(char *filename, char ***lines)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return -1;
    }
    int lines_count = 0;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        lines_count++;
    }
    *lines = (char **)malloc(lines_count * sizeof(char *));
    rewind(file);
    int i = 0;
    while (getline(&line, &len, file) != -1)
    {
        (*lines)[i] = (char *)malloc(len * sizeof(char));
        strcpy((*lines)[i], line);
        i++;
    }
    fclose(file);
    return lines_count;
}
void free_lines(char **lines, int lines_count)
{
    for (int i = 0; i < lines_count; i++)
    {
        free(lines[i]);
    }
    free(lines);
}
char **splitCommands(char *line)
{
    int count = 0;
    char **args = NULL;
    char delims[2] = {' ', '\n'};
    line = strtok(line, delims);
    while (line)
    {
        count++;
        args = realloc(args, count * sizeof(char *));
        args[count - 1] = line;
        line = strtok(NULL, delims);
    }
    args = realloc(args, (count + 1) * sizeof(char *));
    args[count] = NULL;

    return args;
}
void run_line(char *line)
{
    // split by delimiter
    line += 3;
    int pipes[2][2];
    char *commands[MAX_ARGS];
    int i = 0;
    char *command = strtok(line, "|");
    while (command != NULL)
    {
        commands[i++] = command;
        command = strtok(NULL, "|");
    }
    commands[i] = NULL;
    // execute
    for (int j = 0; j < i; j++)
    {
        if (j != 0)
        {
            close(pipes[j % 2][0]);
            close(pipes[j % 2][1]);
        }
        if (pipe(pipes[j % 2]) == -1)
        {
            printf("pipe error\n");
            exit(1);
        }

        // split arguments by space and run command

        pid_t pid = vfork();
        if (pid == 0)
        {
            char *arguments[MAX_ARGS];
            int k = 0;
            char *argument = strtok(commands[j], " \n");
            while (argument != NULL)
            {
                arguments[k++] = argument;
                argument = strtok(NULL, " \n");
            }
            arguments[k] = NULL;

            if (j != i - 1)
            {
                close(pipes[j % 2][0]);
                if (dup2(pipes[j % 2][1], STDOUT_FILENO) < 0)
                {
                    printf("dup21 error\n");
                    exit(1);
                }
            }

            if (j != 0)
            {
                close(pipes[(j + 1) % 2][1]);
                if (dup2(pipes[(j + 1) % 2][0], STDIN_FILENO) < 0)
                {
                    printf("dup22 error\n");
                    exit(1);
                }
            }
            execvp(arguments[0], arguments);
            printf("Error executing command\n");
            exit(1);
        }
        else if (pid < 0)
        {
            printf("Error forking\n");
            exit(1);
        }
        else
        {
            // wait(&status);
        }
    }
    close(pipes[i % 2][0]);
    close(pipes[i % 2][1]);
}
int get_index_of_empy_line(char **lines, int lines_count)
{
    for (int i = 0; i < lines_count; i++)
    {
        if (strcmp(lines[i], "\n") == 0)
        {
            return i;
        }
    }
    return -1;
}
void chain_commands(char *line, char **lines)
{
    int pipes[2][2];

    char *line_nums[MAX_ARGS];
    int i = 0;
    char *command = strtok(line, "|\n");
    while (command != NULL)
    {
        line_nums[i++] = command;
        command = strtok(NULL, "|\n");
    }
    line_nums[i] = NULL;
    for (int j = 0; j < i; j++)
    {
        if (j != 0)
        {
            close(pipes[j % 2][0]);
            close(pipes[j % 2][1]);
        }
        if (pipe(pipes[j % 2]) == -1)
        {
            printf("pipe error\n");
            exit(1);
        }

        int index = atoi(line_nums[j]);
        pid_t pid = vfork();
        if (pid == 0)
        {
            if (j != i - 1)
            {
                close(pipes[j % 2][0]);
                if (dup2(pipes[j % 2][1], STDOUT_FILENO) < 0)
                {
                    printf("dup211 error\n");
                    exit(1);
                }
            }
            // printf("im here\n");
            if (j != 0)
            {
                close(pipes[(j + 1) % 2][1]);
                if (dup2(pipes[(j + 1) % 2][0], STDIN_FILENO) < 0)
                {
                    printf("dup222 error\n");
                    exit(1);
                }
            }
            run_line(lines[index]);
            exit(0);
        }
        else if (pid < 0)
        {
            printf("Error forking\n");
            exit(1);
        }
        else
        {
            // wait(NULL);
        }
    }
    close(pipes[i % 2][0]);
    close(pipes[i % 2][1]);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./main <filename>\n");
        return -1;
    }
    char **lines;
    char *filename = argv[1];
    int lines_count = read_file(filename, &lines);
    if (lines_count == -1)
    {
        return -1;
    }
    // print get_index_of_empy_line

    for (int i = get_index_of_empy_line(lines, lines_count) + 1; i < lines_count; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            chain_commands(lines[i], lines);
            exit(0);
        }
        else if (pid < 0)
        {
            printf("Error forking\n");
            exit(1);
        }
        else
        {
            // wait(NULL);
        }
    }

    // fflush(stdout);

    free_lines(lines, lines_count);
    return 0;
}

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <float.h>

int main(int argc, char *argv[])
{
    int Digs = DECIMAL_DIG;

    if (argc != 3)
    {
        puts("Potrzebne 2 argumenty! ERR");
        return 0;
    }
    long double dx = atof(argv[1]);
    int n = atoi(argv[2]);
    for (int i = 0; i < n; i++)
    {
        long double start = (long double)i / n;
        long double end = (long double)(i + 1) / n;
        pid_t pid = fork();
        if (pid == 0)
        {
            long double res = 0;

            for (long double j = start; j < end; j += dx)
            {
                long double y;
                y = 4 / ((j + dx) * (j + dx) + 1);
                res += dx * y;
            }
            // save to file
            char filepath[20];
            sprintf(filepath, "w%d.txt", i + 1);
            FILE *f_dest = fopen(filepath, "w");
            if (f_dest == NULL)
            {
                puts("Could not open or create the destination file! ERR");
                return 0;
            }
            if (fprintf(f_dest, "%.*Lg\n", Digs, res) < 0)
            {
                puts("Cannot write to the destination file! ERR");
                return 0;
            }
            fclose(f_dest);
            exit(0);
        }
    }
    // poczekaj na procesy i zlicz pliki
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;

    long double result = 0;
    for (int i = 0; i < n; i++)
    {
        long double inter = 0;
        char filepath[20];
        sprintf(filepath, "w%d.txt", i + 1);
        FILE *f_source = fopen(filepath, "r");
        fscanf(f_source, "%Lf", &inter);
        result += inter;
        fclose(f_source);
    }
    printf("Wynik: %.17Lg\n", result);

    return 0;
}
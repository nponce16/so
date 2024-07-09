#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void usage () {
    char buffer[128];
    sprintf(buffer, "Error usage: el programa debe tener dos argumentos ./ej2 num");
    write (1, buffer, strlen(buffer));
    exit(0);
}

void error_and_exit(char *msg, int exit_code) {
    perror(msg);
    exit(exit_code);
}

int main(int argc, char *argv[]) {
    char buffer[128];
    int nhijos = atoi(argv[1]);
    if (argc != 2) usage();


    int ret = fork();
    for (int i = 2; i <= nhijos; ++i) {
        if (ret == 0) {
            sprintf(buffer, "Soy hijo %d y mi PID es: %d\n", i, getpid());
            write(1, buffer, strlen(buffer));
            ret = fork();
        }
        else if(ret == -1) {
            error_and_exit("Error en fork", 1);
        }
    }
    waitpid(-1, NULL, 0);
}
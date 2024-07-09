#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void usage() {
    char buf[128];
    sprintf(buf, "Error en usage: el programa debe recibir 3 argumentos ./busqueda_parcial pid palabra nomfichero\n");
    write(1, buf, strlen(buf));
    exit(0);
}

void error_and_exit(char *msg, int exit_status) {
    perror(msg);
    exit(exit_status);
}

int main(int argc, char *argv[]) {
    char buffer[128];

    if (argc != 4) usage();
    
    int ret = fork();
    switch (ret) {
        case 0:
            execlp("grep", "grep", "-s", argv[2], argv[3], (char*)NULL);
            error_and_exit("Error en execlp", 1);

        case -1:
            error_and_exit("Error en fork", 1);
    }

    int exit_code;
    waitpid(getpid(), &exit_code, 0);
    
    if(WIFEXITED(exit_code)) {
        int statcode = WEXITSTATUS(exit_code);
        sprintf(buffer, "Termina grep palabra %s fichero %s : exit %i\n", argv[2], argv[3], statcode);
        write(1, buffer, strlen(buffer));
    }
    exit(0);
}
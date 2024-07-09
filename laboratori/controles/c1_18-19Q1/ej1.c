#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

char buffer[128];
int sec, temps;
int cont = 0;

void usage() {
    char buffer[128];
    sprintf(buffer, "Error en usage: los parametros deben ser 2 ./ej1 num1 num2\n");
    write(1, buffer, strlen(buffer));
    exit(0);
}

void error_and_exit(char *msg, int exit_status) {
    perror(msg);
    exit(exit_status);
}

void trat(int s) {
    if (s == SIGALRM) {
        ++sec;
        --temps;
        sprintf(buffer, "%d\n", sec);
        write(1, buffer, strlen(buffer));
    }
    else if (s == SIGUSR1) {
        sprintf(buffer, "RESTAN POR ACABAR %d SEGUNDOS\n", temps);
        write(1, buffer, strlen(buffer));
    }
    else if(s == SIGINT) {
        sprintf(buffer, "Numeros mostrados: %d\n", cont);
        write(1, buffer, strlen(buffer));
        exit(cont);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage();
    sec = atoi(argv[1]);
    temps = atoi(argv[2]) - sec;

    sprintf(buffer, "Inicio: %d\n", sec);
    write(1, buffer, strlen(buffer));

    sigset_t mask;
    struct sigaction sh;

    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    sh.sa_handler = trat;
    sh.sa_flags = 0;
    if((sigaction(SIGALRM, &sh, NULL)) < 0) error_and_exit("Error en sigaction", 1);
    if((sigaction(SIGUSR1, &sh, NULL)) < 0) error_and_exit("Error en sigaction", 1);
    if((sigaction(SIGINT, &sh, NULL)) < 0) error_and_exit("Error en sigaction", 1);

    while(sec != atoi(argv[2])) {
        ++cont;
        alarm(1);
        sigfillset(&mask);
        sigdelset(&mask, SIGALRM);
        sigdelset(&mask, SIGINT);
        sigsuspend(&mask);
    }
    sprintf(buffer, "Numeros mostrados: %d\n", cont);
    write(1, buffer, strlen(buffer));
    exit(cont);
}
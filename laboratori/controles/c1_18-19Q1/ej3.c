#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void usage () {
    char buffer[128];
    sprintf(buffer, "Error usage: el programa debe tener tres argumentos ./ej2 n_proc segs");
    write (1, buffer, strlen(buffer));
    exit(0);
}

void error_and_exit(char *msg, int exit_code) {
    perror(msg);
    exit(exit_code);
}

void trat_sigalrm(int s) {
    char buf[128];
    int pid;
    int exit_code;
    int signal_code;

    while ((pid=waitpid(-1, &exit_code, WNOHANG)) > 0){
        kill(pid, SIGKILL);
        if (WIFEXITED(exit_code)) {
            sprintf(buf, "El proceso con pid %d ha finalizado normalmente\n", pid);
            write(1, buf, strlen(buf));
        }
        else {
            signal_code = WTERMSIG(exit_code);
            sprintf(buf, "El proceso con pid %d ha finalizado con el signal %d", pid, signal_code);
            write(1, buf, strlen(buf));
        }
    }
}

int main(int argc, char *argv[]) {
    char buf[128];
    if (argc != 3) usage();

    int n_proc = atoi(argv[1]);
    int segs = atoi(argv[2]);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGCHLD);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction sh;
    sh.sa_handler = trat_sigalrm;
    sh.sa_flags = 0;
    if ((sigaction(SIGALRM, &sh, NULL)) < 0) error_and_exit("Error en sigaction", 1);
    if ((sigaction(SIGCHLD, &sh, NULL)) < 0) error_and_exit("Error en sigaction", 1);

    int ret;
    alarm(segs);
    for(int i = 1; i <= n_proc; ++i) {
        ret = fork();
        if (ret == 0) {
            char str[16];
            sprintf(str, "%d", i);
            execlp("./ej1", "./ej1", str, argv[1], (char*)NULL);
            error_and_exit("Error en execlp", 1);
        }
        else if (ret == -1) {
            error_and_exit("Error en fork", 1);
        }
    }

}
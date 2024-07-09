#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/stat.h>
#include <fcntl.h>
#include <errno.h>

void Usage() {
    char buf[128];
    sprintf(buf, "Error: Usage el programa no ha de tenir cap paràmetre\n");
    write(1, buf, strlen(buf));
    exit(1);
}

void error_and_exit(const char *msg, int exit_status) {
    perror(msg);
    exit(exit_status);
}

int main(int argc, char *argv[]) {
    if (argc != 1) Usage();

    /*
    crea pipe sin nombre
    int fd[2];
    if (pipe(fd) < 0) error_and_exit("Error: pipe", 1);
    */

    if (mknod("pipe1", S_IFIFO| O_TRUNC | 0666, 0) < 0 && (errno != EEXIST)) {
        error_and_exit("Error: mknod", 1);
    }
    
    //input
    int pid = fork();
    if (pid < 0) error_and_exit("Error: fork", 1);
    if (pid == 0) {
        if (close(1) < 0) error_and_exit("Error: close", 1);
        if (open("pipe1", O_WRONLY) < 0) error_and_exit("Error: open", 1);  //ch entrada por pip1
        execlp("ls", "ls", (char*)NULL);
        error_and_exit("Error: execlp", 1);
    }

    //output
    int pid2 = fork();
    if (pid2 < 0) error_and_exit("Error: fork", 1);
    if (pid2 == 0) {
        if (close(0) < 0) error_and_exit("Error: close", 1);
        if (open("pipe1", O_RDONLY) < 0) error_and_exit("Error: open", 1);
        execlp("wc", "wc", (char*)NULL);
        error_and_exit("Error: execlp", 1);
    }

    while (waitpid(-1, NULL, 0) > 0);
    return 0;
}
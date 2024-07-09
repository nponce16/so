#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


void Usage() {
    char buf[128];
    sprintf(buf, "Usage: userprocs ha de tener un parametro que sea el nombre de usuario\n");
    write(1, buf, strlen(buf));
    exit(1);
}

void error_and_exit(char*msg, int exit_code) {
        perror(msg);
        exit(exit_code);
}

int main(int argc, char *argv[]) {
    if (argc != 2) Usage();

    char buf[16];
    char buf2[128];

    int pid, fd1[2];

    pipe(fd1);
    pid = fork();

    if (pid == -1) error_and_exit("Error en fork", 1);
    if (pid == 0) {
        if (dup2(fd1[1], 1) < 0) error_and_exit("Error en dup2", 1);
        if (close(fd1[0]) < 0) error_and_exit("Error en close", 1);
        if (close(fd1[1]) < 0) error_and_exit("Error en close", 1);
        
        execlp("ps", "ps", "-eaf", (char*)NULL);
        error_and_exit("Error en execlp", 1);
    }
    if (close(fd1[1]) < 0) error_and_exit("Error en close", 1);

    int fd2[2];
    pipe(fd2);
    pid = fork();
    if (pid == -1) error_and_exit("Error en fork", 1);
    if (pid == 0) {
        if (dup2(fd1[0], 0) < 0) error_and_exit("Error en dup2", 1);
        if (dup2(fd2[1], 1) < 0) error_and_exit("Error en dup2", 1);
        if (close(fd1[0]) < 0) error_and_exit("Error en close", 1);
        if (close(fd2[0]) < 0) error_and_exit("Error en close", 1);
        if (close(fd2[1]) < 0) error_and_exit("Error en close", 1);
        execlp("grep", "grep", argv[1], (char*)NULL);
        error_and_exit("Error en execlp", 1);
    }

    if (close(fd1[0]) < 0) error_and_exit("Error en close", 1);
    if (close(fd2[1]) < 0) error_and_exit("Error en close", 1);


    int ret;
    ret = mknod("named_pipe", S_IFIFO | 0666, 0);
    if (ret < 0 && errno != EEXIST) error_and_exit("Error en mknod", 1);

    pid = fork();
    if (pid == -1) error_and_exit("Error en fork", 1);
    if (pid == 0) {
        if(dup2(fd2[0], 0) < 0) error_and_exit("Error en dup2", 1);
        if (close(1) < 0) error_and_exit("Error en close", 1);
        if (open("named_pipe", O_RDWR) < 0) error_and_exit("Error en open", 1);
        if (close(fd2[0]) < 0) error_and_exit("Error en close", 1);
        execlp("wc", "wc", "-l", (char*)NULL);
        error_and_exit("Error en execlp", 1);
    }

    if (close(fd2[0]) < 0) error_and_exit("Error en close", 1);

    int fd3;
    if ((fd3 = open("named_pipe", O_RDWR)) < 0) error_and_exit("Error en open", 1);
    
    if ((ret = read(fd3, buf, 16)) < 0) error_and_exit("Error en read", 1);
    buf[ret - 1] = '\0';
    
    sprintf(buf2, "El usuario %s está ejecutando %s procesos\n", argv[1], buf);
    write(1, buf2, strlen(buf2));

    while (waitpid(-1, NULL, 0) > 0);
}
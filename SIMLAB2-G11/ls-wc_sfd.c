#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/socket.h>

#define SOCKET_PATH "/tmp/my_socket"

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

    int sv[2];

    // Crear socket para input
    int input_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (input_socket < 0) {
        error_and_exit("Error: socket", 1);
    }

    struct sockaddr_un input_addr;
    memset(&input_addr, 0, sizeof(struct sockaddr_un));
    input_addr.sun_family = AF_UNIX;
    strcpy(input_addr.sun_path, SOCKET_PATH);

    // Enlazar el socket
    if (bind(input_socket, (struct sockaddr *)&input_addr, sizeof(struct sockaddr_un)) < 0) {
        error_and_exit("Error: bind", 1);
    }

    // Escuchar conexiones entrantes
    if (listen(input_socket, 1) < 0) {
        error_and_exit("Error: listen", 1);
    }

    // Crear socket para output
    int output_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (output_socket < 0) {
        error_and_exit("Error: socket", 1);
    }

    struct sockaddr_un output_addr;
    memset(&output_addr, 0, sizeof(struct sockaddr_un));
    output_addr.sun_family = AF_UNIX;
    strcpy(output_addr.sun_path, SOCKET_PATH);

    // Conectar al socket de input
    if (connect(output_socket, (struct sockaddr *)&output_addr, sizeof(struct sockaddr_un)) < 0) {
        error_and_exit("Error: connect", 1);
    }

    // input
    int pid = fork();
    if (pid < 0) {
        error_and_exit("Error: fork", 1);
    }
    if (pid == 0) {
        close(input_socket);
        dup2(output_socket, 1);
        close(output_socket);
        execlp("ls", "ls", (char*)NULL);
    }

    // output
    int pid2 = fork();
    if (pid2 < 0) {
        error_and_exit("Error: fork", 1);
    }
    if (pid2 == 0) {
        close(output_socket);
        dup2(input_socket, 0);
        close(input_socket);
        execlp("wc", "wc", (char*)NULL);
    }

    close(input_socket);
    close(output_socket);

    waitpid(-1, NULL, 0);

    return 0;
}

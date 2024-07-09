#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage() {
    char buf[128];
    sprintf (buf, "Usage: el programa debe recibir 2 argumentos ./grep palabra nomfichero\n");
    write(1, buf, strlen(buf));
    exit(0);
}

void error_and_exit(char *msg, int exit_status) {
    perror(msg);
    exit(exit_status);
}

int main(int argc, char *argv[]) {
    char buffer[128];

    if (argc != 3) usage();
    exit(0);
    
}
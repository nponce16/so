#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char * argv[]){
    int i, nprocs,ret,st;
    char buf[80];
    nprocs=atoi (argv[1]);
    for (i=0;i<nprocs;i++){
        ret = fork();
        if (ret > 0) {
            sprintf(buf,"%d",nprocs-1);
            if (execlp("./proc","proc",buf,(char *)0) < 0) {
                sprintf(buf, "Error execlp\n");
                write(1,buf,strlen(buf));
            }
        } else {
            sprintf(buf,"Iteracion %d\n",i);
            write(1, buf, strlen(buf));
            exit(i);
        }
    }
    while (waitpid(-1,&st, 0) >0){
        if (WIFEXITED(st)) {
            sprintf(buf,"Acaba hijo %d\n",WEXITSTATUS(st));
            write(1,buf,strlen(buf));
        }
    }
}
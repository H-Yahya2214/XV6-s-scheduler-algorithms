#include "kernel/types.h"
#include "user/user.h"

int main(){
        int fds[2];
        char buf[100];
        int pid;
        pipe(fds);
        pid = fork();
        if(pid == 0){
                int n = read(fds[0], buf, sizeof(buf)); 
                write(1, buf, n); 
                 printf("child\n");
         } else {
                write(fds[1], "saraaa\n",8);
                printf("parent\n");
         }
        exit(0);
}

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
		close(fds[0]);
   		 printf("child\n");
 	 } else {
		write(fds[1], "HI Habiba\n",10);
    		printf("parent\n");
		close(fds[1]);
 	 }
	exit(0);
}

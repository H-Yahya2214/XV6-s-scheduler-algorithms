#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define N_READERS 5
#define N_WRITERS 3
#define MSG_PER_WRITER 10

// Use a file for output synchronization
int output_fd;

void print_sync(char *msg) {
  write(output_fd, msg, strlen(msg));
}

void
reader(int pipe_fd)
{
  char buf[1];
  int n, count = 0;
  int pid = getpid();
  
  printf("[R%d] Reader started\n", pid);
  
  while(1) {
    n = read(pipe_fd, buf, 1);
    if(n <= 0) break;
    count++;
    sleep(1);  // Make thundering herd more visible
    printf("[R%d] Received byte %d\n", pid, count);
  }
  
  printf("[R%d] Finished after reading %d bytes\n", pid, count);
  exit(0);
}

void
writer(int pipe_fd)
{
  char byte = 'A';
  int pid = getpid();
  
  printf("[W%d] Writer started\n", pid);
  
  for(int i = 0; i < MSG_PER_WRITER; i++) {
    write(pipe_fd, &byte, 1);
    printf("[W%d] Wrote byte %d\n", pid, i+1);
    sleep(1);  // Add delay between writes
  }
  
  printf("[W%d] Writer finished\n", pid);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int pipe_fds[2];
  int i, pid;

  // Create output file for synchronized printing
  output_fd = open("output.txt", O_CREATE | O_RDWR);
  if(output_fd < 0) {
    printf("Failed to create output file\n");
    exit(1);
  }

  printf("\n=== Starting wakeup_one() test ===\n\n");

  if(pipe(pipe_fds) < 0) {
    printf("pipe() failed\n");
    exit(1);
  }

  // Create multiple reader processes
  printf("Creating readers...\n");
  for(i = 0; i < N_READERS; i++) {
    pid = fork();
    if(pid < 0) {
      printf("fork() failed\n");
      exit(1);
    }
    if(pid == 0) {
      close(pipe_fds[1]);  // Close write end
      reader(pipe_fds[0]);
    }
    sleep(1); // Add delay between reader creation
  }

  // Create multiple writer processes
  printf("Creating writers...\n");
  for(i = 0; i < N_WRITERS; i++) {
    pid = fork();
    if(pid < 0) {
      printf("fork() failed\n");
      exit(1);
    }
    if(pid == 0) {
      close(pipe_fds[0]);  // Close read end
      writer(pipe_fds[1]);
    }
    sleep(1); // Add delay between writer creation
  }

  // Close both ends in parent
  close(pipe_fds[0]);
  close(pipe_fds[1]);

  // Wait for all children to finish
  for(i = 0; i < N_READERS + N_WRITERS; i++) {
    wait(0);
  }

  printf("\n=== Test completed ===\n");
  
  // Read and display the output file
  close(output_fd);
  output_fd = open("output.txt", O_RDONLY);
  char buf[1024];
  int n;
  while((n = read(output_fd, buf, sizeof(buf))) > 0) {
    write(1, buf, n);  // Write to stdout
  }
  close(output_fd);
  unlink("output.txt");  // Clean up the output file
  
  exit(0);
} 
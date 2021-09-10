#include <stdio.h>
#include <unistd.h>
#include "sys/types.h"

int main() {
    // Fork system call is used for creating a new process, 
    // which is called child process. 
    // After a new child process is created, both processes will
    // execute the next instruction following the fork() system call.
    pid_t processId = fork();

    if(processId == -1) {
        printf("Fork failed.\n");
        return -1;
    }
    else {
        if(!processId)
            printf("Code related to the child process, PID: %i\n", getpid());
        else
            printf("Code related to the parent process, PID: %i\n", getppid());
    }

    return 0;
}
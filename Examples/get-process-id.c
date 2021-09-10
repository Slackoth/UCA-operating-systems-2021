#include <unistd.h>
#include <stdio.h>

int main() {
    int processId = getpid();
    int parentProcessId = getppid();

    printf("The ID of this process is: %i\n", processId);
    printf("The ID of the parent of this process is: %i\n", parentProcessId);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>

void* showMagicNumber() {
    // Cast to void to return
    pthread_exit((void *) 3);
}

int main() {
    pthread_t thread;
    void* magicNumber = NULL;

    // Create thread
    pthread_create(&thread, NULL, showMagicNumber, NULL);

    // Wait for thread to finish
    pthread_join(thread, &magicNumber);

    // Cast to int to print
    printf("%i is a magic number, yes it is!\n", (int) magicNumber);

    return 0;
}
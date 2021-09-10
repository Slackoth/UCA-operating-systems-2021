#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void saySomething() {
    char* message = "Operating";

    for(int i = 0; i < strlen(message); i++) {
        printf("%c", message[i]);

        // typically used for output stream only. Its purpose is to clear 
        // (or flush) the output buffer and move the buffered data to console
        // (in case of stdout) or disk (in case of file output stream). 
        fflush(stdout);

        //uspends the current process for the number of microseconds passed to it.
        usleep(1000);
    }
}

void saySomeOtherThing() {
    char* message = "Systems";

    for(int i = 0; i < strlen(message); i++) {
        printf("%c", message[i]);

        fflush(stdout);
        usleep(1000);
    }
}

int main() {
    pthread_t thread1, thread2;

    printf("\n***BEGINING***\n");
    
    pthread_create(&thread1, NULL, (void *) &saySomething, NULL);
    pthread_join(thread1, NULL);
    pthread_create(&thread2, NULL, (void *) &saySomeOtherThing, NULL);
    pthread_join(thread2, NULL);
    
    printf("\n***END***\n");

    return 0;
}
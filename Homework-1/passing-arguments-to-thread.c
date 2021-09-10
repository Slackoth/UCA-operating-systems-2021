#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>

struct numbers {
    int a;
    int b;
};

void *writeBasicOperations(void *args) {
    // Cast to struct
    struct numbers *nArgs = (struct numbers *) args;

    FILE *file;
    int sum = nArgs->a + nArgs->b;
    int substracion = nArgs->a - nArgs->b;
    int multiplication = nArgs->a * nArgs->b;

    // Create file in write mode
    file = fopen("magicNumbers.txt", "w");

    if(file == NULL) {
        printf("File creation failed.");
        pthread_exit(NULL);
    }

    fprintf(file, "%i,", sum);
    fprintf(file, "%i,", substracion);
    fprintf(file, "%i", multiplication);

    // Close file
    fclose(file);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    struct numbers *args = calloc(sizeof(struct numbers), 1);
    args->a = 7;
    args->b = 3;

    pthread_create(&thread, NULL, writeBasicOperations, args);
    pthread_join(thread, NULL);

    free(args);

    return 0;
}
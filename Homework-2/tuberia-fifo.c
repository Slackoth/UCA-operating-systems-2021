/* Tubería sin nombre entre procesos padre e hijo */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main (){
	int flag, fdChild, fdParent;
	// char* fifoParent = "jepeto", *fifoChild = "pinocchio";
	char* fifoName = "valoo";
	char* pMessage[40], *cMessage = "Sending message from child to parent...";

	flag = mkfifo(fifoName, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

	if(flag != -1) {
		if(fork() == 0) {
			// WRITE
			printf("Waiting parent...\n");
			fdChild = open(fifoName, O_WRONLY);

			if(fdChild == -1)
				perror("Error when creating writing pipe...\n");
			else {
				write(fdChild, cMessage, 40);
				printf("Child is sending this message: %s\n", cMessage);
				close(fdChild);
			}
		}
		else {
			// READ
			fdParent = open(fifoName, O_RDONLY);

			if(fdParent == -1) 
				perror("Error when creating reading pipe...\n");
			else {
				read(fdParent, pMessage, 40);
				printf("Parent is receiving this message: %s\n", pMessage);
				close(fdParent);
			}
		}
	}
	else
		perror("Pipe not created...\n"); 
}
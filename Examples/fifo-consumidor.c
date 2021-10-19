/*consumidor*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	char mensaje[100], *nfifo = "fifo";
	int fd; /*file descriptor*/
	
	printf("Consumidor\n\n");
	/*leyendo en la tuberia (consumidor)*/
	fd = open(nfifo, O_RDONLY);
	if(fd == -1)
		perror("error al abrir la tuberia (lectura)\n");
	else{
		/*lectura de la tuberia*/
		read(fd, mensaje, 100);
		printf("consumidor recibio: %s\n", mensaje);
		close(fd);
	}
	/*eliminando tuberia*/
	unlink(nfifo);
}

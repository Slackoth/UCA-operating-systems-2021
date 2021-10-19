/*productor*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int bandera, fd; /*file descriptor*/
	char *nfifo = "fifo";
	
	printf("Productor\n\n");
	if(argc != 2){
		//mensaje de error a la salida estandar
		fprintf(stderr,"modo de uso: ./productor mensaje\n");
		exit(1);
	}
	/*creando la tuberia*/
	bandera = mkfifo(nfifo, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	if(!bandera){
		/*abriendo la tubería en modo escritura (productor)*/
		printf("productor espera apertura de consumidor\n");
		fd = open(nfifo, O_WRONLY);
		if(fd == -1)
			perror("error al abrir la tuberia (escritura)\n");
		else{
			/*escritura en la tuberia*/
			write(fd, argv[1], 100);
			printf("productor envio: %s\n",argv[1]);
			close(fd);
		}
	}
	else
		perror("error al crear la tuberia\n");
	return 0;
}

/* Tubería sin nombre entre procesos padre e hijo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LEER		0
#define ESCRIBIR	1

int main (){
	int descr[2];	/* Descriptores de ENTRADA y SALIDA de la tubería */
	int bytesleidos;
	char mensaje[100], *frase="La gente positiva cambia el mundo, mientras que la negativa lo mantiene como está";
	
	printf ("Ejemplo de comunicación entre procesos (tuberías) entre padre e hijo.\n");
	pipe (descr);
	if (fork() == 0){
		/*sección del hijo*/
		close (descr[LEER]);
		write (descr[ESCRIBIR], frase, strlen(frase));
		close (descr[ESCRIBIR]);
	}
	else{
		/*sección del padre*/
		close (descr[ESCRIBIR]);
		bytesleidos = read (descr[LEER], mensaje, 100);
		mensaje[bytesleidos] = '\0';
		printf ("Bytes leidos: %d\n", bytesleidos);
		printf ("Mensaje (enviado por el hijo): %s\n", mensaje);
		close (descr[LEER]);
	}
}
/*************************************************************************
	Sistemas Operativos.
	Colas de mensajes. Battleship, jugador2 (bnp2)
	Autor: Carlos Miguel López Loarca

	tipo:
		0 = ver "tipo_mensaje"
		1 = jugador 1 ha iniciado.
		2 = jugador 2 ha iniciado.
	tipo_mensaje:
		0 = esperar al otro jugador.
		1 = turno del otro jugador.
		2 = ganador jugador 1.
		3 = ganador jugador 2.
	
*************************************************************************/

#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define LLAVE 34856

typedef struct mensaje{
	long tipo;
	int m1_fil;
	int m1_col;
	int m2_fil;
	int m2_col;
	int m1_barcos[8];
	int m2_barcos[8];
	int tipo_mensaje;
} mensaje;

char matriz1[4][4];
char matriz2[4][4];
int i, j;
int fil, col;
int barcos = 4; //contador barcos hundidos jugador1
int nbarcos = 0;
int cont = 0;
int bandera = 0;

void mostrarMatrices(void);

int main(){
	//variables necesarias
	int id01;
	int tam;

	//estructuras de mensajes
	mensaje enviar;
	mensaje recibir;

	//inicializando matrices
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			matriz1[i][j]='z';
			matriz2[i][j]='z';
		}
	}

	//muestra los tableros vacios
	mostrarMatrices();
	
	//pide las coordenadas de los barcos para jugador2
	while(nbarcos < 4){
		printf("\nIngrese las coordenadas del barco %d:\n",nbarcos+1);
		printf("fila: ");
		scanf("%d",&fil);		
		printf("columna: ");
		scanf("%d",&col);
		if(fil >= 0 && fil <=3 && col >= 0 && col <=3){
			if(matriz2[fil][col] == '0'){
				printf("casilla ocupada!!!\n");
			}
			else{
				matriz2[fil][col]='0';
				enviar.m2_barcos[cont] = fil;
				cont++;
				enviar.m2_barcos[cont] = col;
				cont++;
				nbarcos++;
				mostrarMatrices();			
			}
		}
	}

	//longitud del mensaje
	tam = sizeof(mensaje) - sizeof(long);

	//inicializando la cola de mensajes
	id01 = msgget(LLAVE,0600|IPC_CREAT);
	
	//esperando a jugador1...
	printf("\nesperando a jugador 1...\n");
	msgrcv(id01,&recibir,tam,1,0); //recibe el mensaje de jugador1
	if(recibir.tipo_mensaje != 0){ //fallo confirmacion de jugador1
		printf("\nerror al iniciar, intente de nuevo....\n");
		msgctl(id01,IPC_RMID,0); //elimina la cola de mensajes
		return 0;
	}

	//indicar que player2 ha iniciado
	enviar.tipo = 2;
	enviar.m1_fil = 0;
	enviar.m1_col = 0;
	enviar.m2_fil = 0;
	enviar.m2_col = 0;
	enviar.tipo_mensaje = 0;

	//envia el mensaje al jugador1
	msgsnd(id01,&enviar,tam,0);
	
	//muestra los tableros
	mostrarMatrices();	

	//esperando al jugador 1
	printf("\nturno de jugador 1...\n");
	msgrcv(id01,&recibir,tam,0,0);
	//verifica si hundio un barco del jugador2
	if(matriz2[recibir.m2_fil][recibir.m2_col] == '0'){
		matriz2[recibir.m2_fil][recibir.m2_col]='x'; //barco hundido
	}else{
		matriz2[recibir.m2_fil][recibir.m2_col]='#'; //ataque fallido
	}
	mostrarMatrices();
		
	while(1){
		printf("\nIngrese las coordenadas de ataque:\n");
		printf("fila: ");
		scanf("%d",&fil);		
		printf("columna: ");
		scanf("%d",&col);	
		if(fil >= 0 && fil <=3 && col >= 0 && col <=3){
			if(matriz1[fil][col] == 'x' || matriz1[fil][col] == '#'){
				printf("casilla ocupada!!!\n");
			}
			else{	//verifica si hundio un barco del jugador1
				for(i=0; i<8; i+=2){
					if(recibir.m1_barcos[i]==fil && recibir.m1_barcos[i+1]==col){
						matriz1[fil][col]='x'; //barco hundido
						bandera = 1;
						barcos--; //disminuye el contador de barcos		
					}
				}
				if(bandera == 0){
					matriz1[fil][col]='#'; //ataque fallido
				}
				bandera = 0;
				mostrarMatrices();
				
				//verificando si gano el jugador2
				if(barcos == 0){
					printf("\n¡¡ganador jugador 2!!\n");
					enviar.m1_fil = fil;
					enviar.m1_col = col;
					enviar.tipo_mensaje = 3;
					msgsnd(id01,&enviar,tam,0);
					return 0;
				}

				//turno de jugador 2
				enviar.tipo = 1;
				enviar.m1_fil = fil;
				enviar.m1_col = col;
				enviar.tipo_mensaje = 1;
				msgsnd(id01,&enviar,tam,0);
			
				//esperando al jugador 1
				printf("\nturno de jugador 1...\n");
				msgrcv(id01,&recibir,tam,0,0);
				switch(recibir.tipo_mensaje){
					case 1:	//verifica si hundio un barco del jugador2
						if(matriz2[recibir.m2_fil][recibir.m2_col] == '0'){
							matriz2[recibir.m2_fil][recibir.m2_col]='x'; //barco hundido
						}else{
							matriz2[recibir.m2_fil][recibir.m2_col]='#'; //ataque fallido
						}
						mostrarMatrices();
						break;
					case 2:	matriz2[recibir.m2_fil][recibir.m2_col]='x'; //barco hundido
						mostrarMatrices();
						printf("\n¡¡ganador jugador 1!!\n");
						msgctl(id01, IPC_RMID, 0);
						return 0;
						break;
				}
			
			}
		}
		else{
			printf("¡la casilla (%d,%d) no existe!\n",fil, col);
		}
	}
	
}

void mostrarMatrices(void){
	system("clear");
	printf("JUGADOR 2 - Batalla Naval\n");
	printf("---------------------------------------------\n");
	printf("Mi tablero                Ataques\n");
	printf("    0   1   2   3             0   1   2   3\n");
	for(i=0; i<4; i++){
		//Mi tablero
		printf("%d ", i);
		printf("| ");
		for(j=0; j<4; j++){
			if(matriz2[i][j]=='z'){
				printf("  | ");
			}
			else{
				printf("%c | ",matriz2[i][j]);
			}
		}
		//Tablero de ataque
		printf("      ");
		printf("%d ", i);
		printf("| ");
		for(j=0; j<4; j++){
			if(matriz1[i][j]=='z'){
				printf("  | ");
			}
			else{
				printf("%c | ",matriz1[i][j]);
			}
		}
		printf("\n");
	}
}
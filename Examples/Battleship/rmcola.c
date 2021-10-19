#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define LLAVE 34856

int main(){
	int id01;
	id01 = msgget(LLAVE,0600|IPC_CREAT);
	msgctl(id01,IPC_RMID,0); //elimina la cola de mensajes
	printf("\ncola eliminada ok\n");
	return 0;
}
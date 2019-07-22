#ifndef UTILGUENGUENCHA_COMUNICACION_H_
#define UTILGUENGUENCHA_COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <error.h>
#include "utils.h"
#include <semaphore.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#define BACKLOG 10     // Cuántas conexiones pendientes se mantienen en cola

/**
* @NAME: retornarControl
* @DESC: Es la funcion que llaman cuando reciben una instruccion por servidor_comunicacion()
*/
void retornarControl(Instruccion *instruccion, int socket_cliente);
/**
* @NAME: servidor_comunicacion
* @DESC: Funcion que levanta un servicio de servidor de comunicacion para recibir instrucciones
*/
void servidor_comunicacion(Comunicacion *comunicacion);
/**
* @NAME: enviar_instruccion
* @DESC: envia la instruccion y libera la misma, retorna la instruccion correspondiente a la respuesta
*/
Instruccion *enviar_instruccion(char* ip, char* puerto, Instruccion *instruccion, Procesos proceso_del_que_envio, Tipo_Comunicacion tipo_comu);
/**
* @NAME: responder
* @DESC: responde al fd asociado y devuelve la instruccion SUCCESS O ERROR segun corresponda
*/
Instruccion *responder(int fd_a_responder, Instruccion *instruccion);
/**
* @NAME: respuesta_error
* @DESC: devuelve la estructura de una instruccion con el error seteado
*/
Instruccion *respuesta_error(Error_set error);
/**
* @NAME: respuesta_success
* @DESC: devuelve la estructura de una instruccion con el success
*/
Instruccion *respuesta_success(void);
#endif /* UTILGUENGUENCHA_COMUNICACION_H_ */

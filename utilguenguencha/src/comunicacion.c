#include "comunicacion.h"
#include<pthread.h>
#include <time.h>

pthread_mutex_t mutex_diccionario_fd; // Lock para estructura de diccionario
pthread_mutex_t mutex_escucha;

Procesos proceso_servidor;

typedef struct {
	int fd;
	pthread_mutex_t mutex;
}Connection;


/**
* @NAME: iniciar_servidor
* @DESC: Es la funcion que inicia los parametros para recibir en localhost:puerto
*/
int iniciar_servidor(char* puerto);
/**
* @NAME: crear_conexion
* @DESC: Funcion que crea una conexion con la IP y Puerto y devuelve fd asociado
*/
Connection *crear_conexion(char *ip, char* puerto);
/**
* @NAME: crear_paquete
* @DESC: Funcion que crea un paquete con los datos suministrados
*/
t_paquete* crear_paquete(Tipo_Comunicacion tipo_comu, Procesos proceso_del_que_envio, Instruccion* instruccion);
/**
* @NAME: crear_paquete_retorno
* @DESC: Crea un paquete de retorno para enviar
*/
t_paquete_retorno *crear_paquete_retorno(Instruccion *instruccion);
/**
* @NAME: enviar_paquete
* @DESC: Funcion que serializa el paquete y lo envia al fd indicado
*/
bool enviar_paquete(t_paquete* paquete, int socket_cliente);
/**
* @NAME: liberar_conexion
* @DESC: Cierra el fd enviado por parametro
*/
void liberar_conexion(int socket_cliente);
/**
* @NAME: eliminar_paquete
* @DESC: Vacia las estructuras de un paquete y libera memoria
*/
void eliminar_paquete(t_paquete* paquete);
/**
* @NAME: eliminar_paquete_retorno()
* @DESC: Borra las estrucutas del paquete
*/
void eliminar_paquete_retorno(t_paquete_retorno* paquete);
/**
* @NAME: empaquetar_select
* @DESC: Empaqueta la estructura de un select en el paquete enviado
*/
void empaquetar_select(t_paquete *paquete, Select *select);
/**
* @NAME: empaquetar_insert
* @DESC: Empaqueta la estructura de un insert en el paquete enviado
*/
void empaquetar_insert(t_paquete *paquete, Insert *insert);
/**
* @NAME: empaquetar_create
* @DESC: Empaqueta la estructura de un create en el paquete enviado
*/
void empaquetar_create(t_paquete * paquete, Create *create);
/**
* @NAME: empaquetar_describe
* @DESC: Empaqueta la estructura de un describe en el paquete enviado
*/
void empaquetar_describe(t_paquete * paquete, Describe *describe);
/**
* @NAME: empaquetar_drop
* @DESC: Empaqueta la estructura de un drop en el paquete enviado
*/
void empaquetar_drop(t_paquete * paquete, Drop * drop);
/**
* @NAME: empaquetar_journal
* @DESC: Empaqueta la estructura de un journal en el paquete enviado
*/
void empaquetar_journal(t_paquete * paquete, Journal * journal);
/**
* @NAME: empaquetar_gossip
* @DESC: Empaqueta la estructura de un gossip en el paquete enviado
*/
void empaquetar_gossip(t_paquete * paquete, Gossip * gossip);
/**
* @NAME: serializar_paquete
* @DESC: Devuelve un puntero a un stream en el que se contiene todo
*/
bool recibir_buffer(int aux1, Instruccion *instruccion, Tipo_Comunicacion tipo_comu);
/**
* @NAME: desempaquetar_select
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Select *desempaquetar_select(void* stream);
/**
* @NAME: desempaquetar_insert
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Insert *desempaquetar_insert(void* stream);
/**
* @NAME: desempaquetar_create
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Create *desempaquetar_create(void* stream);
/**
* @NAME: desempaquetar_describe
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Describe *desempaquetar_describe(void* stream);
/**
* @NAME: desempaquetar_drop
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Drop *desempaquetar_drop(void* stream);
/**
* @NAME: desempaquetar_journal
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Journal *desempaquetar_journal(void* stream);
/**
* @NAME: desempaquetar_gossip
* @DESC: Desempaqueta lo recibido en stream y lo mete en la estructura
*/
Gossip *desempaquetar_gossip(void* stream);
/**
* @NAME: validar_sender
* @DESC: Valida el sender segun el tipo de comunicacion, el sender y el receiver
*/
bool validar_sender(Procesos sender, Procesos receiver, Tipo_Comunicacion comunicacion);
/**
* @NAME: recibir_respuesta
* @DESC: Se bloquea (Temporalmente) esperando respuesta de a quien le envie
*/
Instruccion *recibir_respuesta(int fd_a_escuchar);
/**
* @NAME: recibir_retorno
* @DESC: Recibe Retorno y devuelve el la instruccion
*/
Instruccion *recibir_retorno(int fd_a_escuchar);
/**
* @NAME: armar_retorno_value
* @DESC: Devuelve la instruccion correspondiente a Retorno con value
*/
Instruccion *armar_retorno_value(void *chunk);
/**
* @NAME: recibir_error
* @DESC: Recibe el error correspondiente en el fd que le enviamos por parametro
*/
Instruccion *recibir_error(int fd_a_escuchar);
/**
* @NAME: armar_retorno_gossip()
* @DESC: Arma el retorno para el gossip
*/
Instruccion *armar_retorno_gossip(void *chunk);
/**
* @NAME: serializar_paquete_retorno
* @DESC: Devuelve un puntero a un stream en el que se contiene todo
*/
bool enviar_paquete_retorno(t_paquete_retorno* paquete, int socket_cliente);
/**
* @NAME: armar_retorno_max_value()
* @DESC: Devuelve una instruccion con el MAX_VALUE seteado
*/
Instruccion *armar_retorno_max_value(void *chunk);
/**
* @NAME: armar_retorno_describe()
* @DESC: Recibe una lista de describes y devuelve una instruccion de retorno
*/
Instruccion *armar_retorno_describe(void *chunk);
/**
* @NAME: empaquetar_retorno_valor()
* @DESC: Mete en el paquete los datos necesarios
*/
void empaquetar_retorno_valor(t_paquete_retorno *paquete, Retorno_Value *ret_val);
/**
* @NAME: empaquetar_retorno_describe()
* @DESC: Mete en el paquete los datos necesarios
*/
void empaquetar_retorno_describe(t_paquete_retorno *paquete, Describes *describes);
/**
* @NAME: empaquetar_retorno_max_val()
* @DESC: Mete en el paquete los datos necesarios
*/
void empaquetar_retorno_max_val(t_paquete_retorno *paquete, Retorno_Max_Value *max_val);
/**
* @NAME: empaquetar_retorno_error()
* @DESC: Mete en el paquete los datos necesarios
*/
void empaquetar_retorno_error(t_paquete_retorno *paquete, Error *error);
/**
* @NAME: empaquetar_retorno_gossip()
* @DESC: Mete en el paquete los datos necesarios
*/
void empaquetar_retorno_gossip(t_paquete_retorno *paquete, Gossip *ret_gos);
/**
* @NAME: empaquetar_retorno_success()
* @DESC: Empaqueta un success
*/
void empaquetar_retorno_success(t_paquete_retorno *paquete);
/**
* @NAME: get_conn()
* @DESC: busca el fd correspondiente a ip y puerto dados si no esta da NULL
*/
Connection *get_conn(char* ip, char* puerto);
/**
* @NAME: update_conn()
* @DESC: pone el fd en el diccionario y devuelve un puntero al mismo
*/
Connection *update_conn(char *ip, char *puerto, Connection *conn);
/**
* @NAME: desafectar_conn()
* @DESC: desafecta una conececcion del listado de disponibles
*/
void desafectar_conn(char* ip, char* puerto);
/**
* @NAME: escucha()
* @DESC: hilo que atiende los llamados de un fd mientras esta comunicacion este viva
*/
void escucha(int *valor);

void desafectar_conn(char* ip, char* puerto){
	char *key = string_new();
	string_append(&key, ip);
	string_append(&key, puerto);

	pthread_mutex_lock(&mutex_diccionario_fd);
	Connection* conn_old = dictionary_remove(fd_disponibles, key);
	pthread_mutex_unlock(&mutex_diccionario_fd);

	list_add(fd_desafectados, conn_old);

	free(key);

}



Connection *get_conn(char *ip, char *puerto){
	char *key = string_new();
	string_append(&key, ip);
	string_append(&key, puerto);
	pthread_mutex_lock(&mutex_diccionario_fd);
	Connection *result = dictionary_get(fd_disponibles, key);
	pthread_mutex_unlock(&mutex_diccionario_fd);
	free(key);
	return result;
}


Connection *update_conn(char *ip, char *puerto, Connection *new_conn){
	char *key = string_new();
	string_append(&key, ip);
	string_append(&key, puerto);
	pthread_mutex_lock(&mutex_diccionario_fd);
	Connection *old_conn =dictionary_get(fd_disponibles, key);
	if(old_conn == NULL){
		dictionary_put(fd_disponibles, key, new_conn);
		old_conn = new_conn;
	}else{
		old_conn->fd = new_conn->fd;
	}
	pthread_mutex_unlock(&mutex_diccionario_fd);
	free(key);
	return old_conn;
}

void servidor_comunicacion(Comunicacion *comunicacion){
	pthread_mutex_init(&mutex_escucha,NULL);
	fd_set fd_set_master, fd_set_temporal;
	int aux1, fd_max, server_socket;
	server_socket = iniciar_servidor(comunicacion->puerto_servidor);
	proceso_servidor = comunicacion->proceso;
	free(comunicacion->puerto_servidor);
	free(comunicacion);
	FD_ZERO(&fd_set_master);
	FD_SET(server_socket, &fd_set_master);
	fd_max = server_socket;
	for (;;) {
		FD_ZERO(&fd_set_temporal);
		fd_set_temporal = fd_set_master;
		if (select(fd_max + 1, &fd_set_temporal, NULL, NULL, NULL) == -1) {
			log_error(LOG_ERROR,"Error en el select");
			continue;
		}
		int fin = fd_max;
		for (aux1 = 0; aux1 <= fin; aux1++) {
			if (FD_ISSET(aux1, &fd_set_temporal)) {
				if (aux1 == server_socket) {
					struct sockaddr_in client_address;
					size_t tamanio_client_address = sizeof(client_address);
					int socket_cliente = accept(server_socket, (struct sockaddr *) &client_address, &tamanio_client_address);
					if (socket_cliente < 0) {
						log_error(LOG_ERROR, "Error levantando un socket para cliente");
						continue;
					}
					FD_SET(socket_cliente, &fd_set_master);
					if (socket_cliente > fd_max) {
						fd_max = socket_cliente;
					}
				} else {
					FD_CLR(aux1, &fd_set_master);
					pthread_t hilo_escucha;
					int *valor = malloc(sizeof(int));
					*valor = aux1;
					pthread_create(&hilo_escucha, NULL, (void*)escucha, valor);
					pthread_detach(hilo_escucha);
				}
			}			
		}
	}
}

void escucha(int *valor){
	int aux1 = *valor;
	free(valor);
	for(;;){
		Tipo_Comunicacion tipo_comu;
		if ((recv(aux1, &tipo_comu, sizeof(Tipo_Comunicacion), MSG_WAITALL)) <= 0) {
			liberar_conexion(aux1);
			break;
		} else {
			Procesos proceso_que_envia;
			if ((recv(aux1, &proceso_que_envia, sizeof(Procesos), MSG_WAITALL)) <= 0) {
				liberar_conexion(aux1);
				break;
			}else if(validar_sender(proceso_que_envia, proceso_servidor, tipo_comu)){
				Instruccion *instruccion = malloc(sizeof(Instruccion));
				if ((recv(aux1, &instruccion->instruccion, sizeof(Instruction_set), MSG_WAITALL))<= 0) {
					free(instruccion);
					liberar_conexion(aux1);
					break;
				}
				if (recibir_buffer(aux1, instruccion, tipo_comu)) {
					fsync(aux1);
					pthread_mutex_lock(&mutex_escucha);
					retornarControl(instruccion, aux1);
					pthread_mutex_unlock(&mutex_escucha);
					
				} else {
					fsync(aux1);
					free(instruccion);
					break;
				}
			}else{
				liberar_conexion(aux1);
				break;
			}
		}
	}
}

int iniciar_servidor(char* puerto_servidor) {
	struct addrinfo hints, *res;
	int socket_servidor;
	int activado = 1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, puerto_servidor, &hints, &res);
	if((socket_servidor = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != -1){
		if(setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado)) != -1){
			if(bind(socket_servidor, res->ai_addr, res->ai_addrlen) != -1){
				if(listen(socket_servidor, BACKLOG) != -1){
					freeaddrinfo(res);
					return socket_servidor;
				}
			}
		}	
	}
	log_error(LOG_ERROR, "Fallo al levantar servidor");
	exit_gracefully(EXIT_FAILURE);
	return EXIT_FAILURE;
}

bool recibir_buffer(int aux1, Instruccion *instruccion, Tipo_Comunicacion tipo_comu) {
	size_t buffer_size;
	void* stream;
	if ((recv(aux1, &buffer_size, sizeof(size_t), MSG_WAITALL)) <= 0) {
		return false;
	}
	if (buffer_size > 0){
		stream = malloc(buffer_size);
	}
	switch (instruccion->instruccion) {
	case SELECT:
		if(tipo_comu == T_INSTRUCCION){
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Select *select;
			select = desempaquetar_select(stream);
			instruccion->instruccion = SELECT;
			instruccion->instruccion_a_realizar = select;
			free(stream);
			return true;
		}else{
			break;
		}
	case INSERT:
		if(tipo_comu == T_INSTRUCCION){
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Insert *insert;
			insert = desempaquetar_insert(stream);
			instruccion->instruccion = INSERT;
			instruccion->instruccion_a_realizar = insert;
			free(stream);
			return true;
		}else{
			break;
		}
	case CREATE:
		if(tipo_comu == T_INSTRUCCION){
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Create *create;
			create = desempaquetar_create(stream);
			instruccion->instruccion = CREATE;
			instruccion->instruccion_a_realizar = create;
			free(stream);
			return true;
		}else{
			break;
		}
	case DESCRIBE:
		if(tipo_comu == T_INSTRUCCION){
			if(buffer_size == 0){
				instruccion->instruccion = DESCRIBE;
				Describe *describe = malloc(sizeof(Describe));
				describe->nombre_tabla = NULL;
				instruccion->instruccion_a_realizar = describe;
				return true;
			}
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Describe *describe;
			describe = desempaquetar_describe(stream);
			instruccion->instruccion = DESCRIBE;
			instruccion->instruccion_a_realizar = describe;
			free(stream);
			return true;
		}else{
			break;
		}
	case DROP:
		if(tipo_comu == T_INSTRUCCION){
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Drop *drop;
			drop = desempaquetar_drop(stream);
			instruccion->instruccion = DROP;
			instruccion->instruccion_a_realizar = drop;
			free(stream);
			return true;
		}else{
			break;
		}
	case JOURNAL:
		if(tipo_comu == T_INSTRUCCION){
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL))<= 0) {
				break;
			}
			Journal *journal;
			journal = desempaquetar_journal(stream);
			instruccion->instruccion = JOURNAL;
			instruccion->instruccion_a_realizar = journal;
			free(stream);
			return true;
		}else{
			break;
		}
	case GOSSIP:
		if(tipo_comu == T_GOSSIPING){
			if(buffer_size == 0){
				instruccion->instruccion = GOSSIP;
				Gossip *gossip = malloc(sizeof(GOSSIP));
				gossip->lista_memorias = list_create();
				instruccion->instruccion_a_realizar = gossip;
				return true;
			}
			if ((recv(aux1, stream, buffer_size, MSG_WAITALL)) <= 0) {
				break;
			}
			Gossip *gossip;
			gossip = desempaquetar_gossip(stream);
			instruccion->instruccion = GOSSIP;
			instruccion->instruccion_a_realizar = gossip;
			free(stream);
			return true;
		}else{
			break;
		}
	case MAX_VALUE:
		if(tipo_comu == T_VALUE){
			instruccion->instruccion = MAX_VALUE;
			return true;
		}else{
			return false;
		}
	default:
		break;
	}
	if (buffer_size > 0){
		free(stream);
	}
	return false;
}

Instruccion *enviar_instruccion(char* ip, char* puerto, Instruccion *instruccion, Procesos proceso_del_que_envio, Tipo_Comunicacion tipo_comu) {
	Connection *conn = crear_conexion(ip, puerto);
	t_paquete * paquete = crear_paquete(tipo_comu, proceso_del_que_envio, instruccion);
	if(conn != NULL){
		pthread_mutex_lock(&conn->mutex);
		if (enviar_paquete(paquete, conn->fd)) {
			eliminar_paquete(paquete);
			Instruccion *respuesta = recibir_respuesta(conn->fd);
			fsync(conn->fd);
			pthread_mutex_unlock(&conn->mutex);
			return respuesta;
		}else{
			fsync(conn->fd);
			pthread_mutex_unlock(&conn->mutex);
			desafectar_conn(ip, puerto);
		}
	}
	eliminar_paquete(paquete);
	return respuesta_error(CONNECTION_ERROR);
}

Connection *crear_conexion(char *ip, char* puerto) {
	Connection *conn = get_conn(ip, puerto);
	if(conn == NULL){
		conn = malloc(sizeof(Connection));
		conn->fd = -1;
		pthread_mutex_init(&conn->mutex, NULL);
	}
	if(conn->fd == -1){
		int sockfd;
	    struct sockaddr_in servaddr;
	    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	    	pthread_mutex_destroy(&conn->mutex);
			free(conn);
	        return NULL;
	    }
	    bzero(&servaddr, sizeof(servaddr));

	    servaddr.sin_family = AF_INET;
	    servaddr.sin_addr.s_addr = inet_addr(ip);
	    servaddr.sin_port = htons(atoi(puerto));

	    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
			pthread_mutex_destroy(&conn->mutex);
			free(conn);
	        return NULL;
	    }
	    conn->fd = sockfd;
    	return update_conn(ip, puerto, conn);
    }

	return conn;
}

bool enviar_paquete(t_paquete* paquete, int socket_cliente) {
	size_t desplazamiento = 0;
	void *a_enviar = malloc(sizeof(paquete->comunicacion) + sizeof(paquete->source) + sizeof(paquete->header) + sizeof(paquete->buffer->size));
	memcpy(a_enviar + desplazamiento, &paquete->comunicacion, sizeof(paquete->comunicacion));
	desplazamiento += sizeof(paquete->comunicacion);
	memcpy(a_enviar + desplazamiento, &paquete->source, sizeof(paquete->source));
	desplazamiento += sizeof(paquete->source);
	memcpy(a_enviar + desplazamiento, &paquete->header, sizeof(paquete->header));
	desplazamiento += sizeof(paquete->header);
	memcpy(a_enviar + desplazamiento, &paquete->buffer->size, sizeof(paquete->buffer->size));
	desplazamiento += sizeof(paquete->buffer->size);
	if(paquete->buffer->size == 0){
		if ((send(socket_cliente, a_enviar, desplazamiento, 0)) < 0) {
			free(a_enviar);
			return false;
		}else{
			free(a_enviar);
			return true;
		}
	}else{
		a_enviar = realloc(a_enviar, desplazamiento + paquete->buffer->size);
		memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
		desplazamiento += paquete->buffer->size;
		if ((send(socket_cliente, a_enviar, desplazamiento, 0)) < 0) {
			free(a_enviar);
			return false;
		}else{
			free(a_enviar);
			return true;
		}
	}
}

bool enviar_paquete_retorno(t_paquete_retorno* paquete, int socket_cliente) {
	size_t desplazamiento = 0;
	void*a_enviar = malloc(sizeof(paquete->header) + sizeof(paquete->buffer->size) + paquete->buffer->size);
	memcpy(a_enviar + desplazamiento, &paquete->header, sizeof(paquete->header));
	desplazamiento += sizeof(paquete->header);
	memcpy(a_enviar + desplazamiento, &paquete->buffer->size, sizeof(paquete->buffer->size));
	desplazamiento += sizeof(paquete->buffer->size);
	memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;
	if ((send(socket_cliente, a_enviar, desplazamiento, 0)) < 0) {
		free(a_enviar);
		return false;
	}else{
		free(a_enviar);
		return true;
	}
}

void liberar_conexion(int socket_cliente) {
	fsync(socket_cliente);
	close(socket_cliente);
}

void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void crear_buffer_retorno(t_paquete_retorno* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void eliminar_paquete_retorno(t_paquete_retorno* paquete){
free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_paquete* crear_paquete(Tipo_Comunicacion tipo_comu, Procesos proceso_del_que_envio, Instruccion* instruccion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->comunicacion = tipo_comu;
	paquete->source = proceso_del_que_envio;
	paquete->header = instruccion->instruccion;
	crear_buffer(paquete);
	switch (instruccion->instruccion) {
	case SELECT:
		empaquetar_select(paquete, (Select*) instruccion->instruccion_a_realizar);
		break;
	case INSERT:
		empaquetar_insert(paquete, (Insert*) instruccion->instruccion_a_realizar);
		break;
	case CREATE:
		empaquetar_create(paquete, (Create*) instruccion->instruccion_a_realizar);
		break;
	case DESCRIBE:
		empaquetar_describe(paquete, (Describe*) instruccion->instruccion_a_realizar);
		break;
	case DROP:
		empaquetar_drop(paquete, (Drop*) instruccion->instruccion_a_realizar);
		break;
	case JOURNAL:
		empaquetar_journal(paquete, (Journal*) instruccion->instruccion_a_realizar);
		break;
	case GOSSIP:
		empaquetar_gossip(paquete, (Gossip*) instruccion->instruccion_a_realizar);
		break;
	default:
		// Al default el unico que debe llegar es el MAX_VAL porque es el unico que usa esta opcion
		// y ya con el header del otro lado entendemos que lo que se pide es un MAX_VAL
		break;
	}
	free(instruccion);
	return paquete;
}

t_paquete_retorno *crear_paquete_retorno(Instruccion *instruccion){
	t_paquete_retorno *paquete = malloc(sizeof(t_paquete_retorno));
	paquete->header = instruccion->instruccion;
	crear_buffer_retorno(paquete);
	switch (instruccion->instruccion) {
		case RETORNO:;
			Retorno_Generico *retorno = instruccion->instruccion_a_realizar;
			switch(retorno->tipo_retorno){
				case VALOR:
					empaquetar_retorno_valor(paquete, (Retorno_Value*) retorno->retorno);
					break;
				case DATOS_DESCRIBE:
					empaquetar_retorno_describe(paquete, (Describes *)retorno->retorno);
					break;
				case TAMANIO_VALOR_MAXIMO:
					empaquetar_retorno_max_val(paquete, (Retorno_Max_Value *)retorno->retorno);
					break;
				case RETORNO_GOSSIP:
					empaquetar_retorno_gossip(paquete, (Gossip *)retorno->retorno);
					break;
				case SUCCESS:
					empaquetar_retorno_success(paquete);
					//free(retorno->retorno);
					break;
			}
			break;
		
		default:;
			Error *error = instruccion->instruccion_a_realizar;
			empaquetar_retorno_error(paquete, error);
			break;
	}
	free(instruccion->instruccion_a_realizar);
	free(instruccion);
	return paquete;
}

void empaquetar_select(t_paquete *paquete, Select *select) {
	size_t tamanio_nombre_tabla = (strlen(select->nombre_tabla) + 1);
	paquete->buffer->stream = malloc(sizeof(select->key) + sizeof(size_t) + tamanio_nombre_tabla + sizeof(select->timestamp));
	memcpy(paquete->buffer->stream, &select->key, sizeof(select->key));
	paquete->buffer->size += sizeof(select->key);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
	paquete->buffer->size += sizeof(tamanio_nombre_tabla);
	memcpy(paquete->buffer->stream + paquete->buffer->size, select->nombre_tabla, tamanio_nombre_tabla);
	paquete->buffer->size += tamanio_nombre_tabla;
	memcpy(paquete->buffer->stream + paquete->buffer->size, &select->timestamp, sizeof(select->timestamp));
	paquete->buffer->size += sizeof(select->timestamp);
	free(select->nombre_tabla);
	free(select);
}

void empaquetar_insert(t_paquete *paquete, Insert *insert) {
	size_t tamanio_nombre_tabla = (strlen(insert->nombre_tabla) + 1);
	size_t tamanio_value = (strlen(insert->value) + 1);
	paquete->buffer->stream = malloc(sizeof(insert->key) + sizeof(size_t) + tamanio_nombre_tabla + sizeof(insert->timestamp)+ sizeof(insert->timestamp_insert) + sizeof(size_t) + tamanio_value);
	memcpy(paquete->buffer->stream, &insert->key, sizeof(insert->key));
	paquete->buffer->size += sizeof(insert->key);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
	paquete->buffer->size += sizeof(tamanio_nombre_tabla);
	memcpy(paquete->buffer->stream + paquete->buffer->size, insert->nombre_tabla, tamanio_nombre_tabla);
	paquete->buffer->size += tamanio_nombre_tabla;
	memcpy(paquete->buffer->stream + paquete->buffer->size, &insert->timestamp, sizeof(insert->timestamp));
	paquete->buffer->size += sizeof(insert->timestamp);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &insert->timestamp_insert, sizeof(insert->timestamp_insert));
	paquete->buffer->size += sizeof(insert->timestamp_insert);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_value, sizeof(tamanio_value));
	paquete->buffer->size += sizeof(tamanio_value);
	memcpy(paquete->buffer->stream + paquete->buffer->size, insert->value, tamanio_value);
	paquete->buffer->size += tamanio_value;
	free(insert->nombre_tabla);
	free(insert->value);
	free(insert);
}

void empaquetar_create(t_paquete * paquete, Create *create) {
	size_t tamanio_nombre_tabla = (strlen(create->nombre_tabla) + 1);
	paquete->buffer->stream = malloc(sizeof(create->compactation_time) + sizeof(create->consistencia) + sizeof(size_t) + tamanio_nombre_tabla + sizeof(create->particiones) + sizeof(create->timestamp));
	memcpy(paquete->buffer->stream, &create->compactation_time, sizeof(create->compactation_time));
	paquete->buffer->size += sizeof(create->compactation_time);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &create->consistencia, sizeof(create->consistencia));
	paquete->buffer->size += sizeof(create->consistencia);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
	paquete->buffer->size += sizeof(tamanio_nombre_tabla);
	memcpy(paquete->buffer->stream + paquete->buffer->size, create->nombre_tabla, tamanio_nombre_tabla);
	paquete->buffer->size += tamanio_nombre_tabla;
	memcpy(paquete->buffer->stream + paquete->buffer->size, &create->particiones, sizeof(create->particiones));
	paquete->buffer->size += sizeof(create->particiones);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &create->timestamp, sizeof(create->timestamp));
	paquete->buffer->size += sizeof(create->timestamp);
	free(create->nombre_tabla);
	free(create);
}

void empaquetar_describe(t_paquete * paquete, Describe *describe) {
	if(describe->nombre_tabla == NULL){
		free(describe);
	}else{
		size_t tamanio_nombre_tabla = (strlen(describe->nombre_tabla) + 1);
		paquete->buffer->stream = malloc( sizeof(size_t) + tamanio_nombre_tabla + sizeof(describe->timestamp));
		memcpy(paquete->buffer->stream, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
		paquete->buffer->size += sizeof(tamanio_nombre_tabla);
		memcpy(paquete->buffer->stream + paquete->buffer->size, describe->nombre_tabla, tamanio_nombre_tabla);
		paquete->buffer->size += tamanio_nombre_tabla;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &describe->timestamp, sizeof(describe->timestamp));
		paquete->buffer->size += sizeof(describe->timestamp);
		free(describe->nombre_tabla);
		free(describe);
	}
}

void empaquetar_drop(t_paquete * paquete, Drop * drop) {
	size_t tamanio_nombre_tabla = (strlen(drop->nombre_tabla) + 1);
	paquete->buffer->stream = malloc( sizeof(size_t) + tamanio_nombre_tabla + sizeof(drop->timestamp));
	memcpy(paquete->buffer->stream, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
	paquete->buffer->size += sizeof(tamanio_nombre_tabla);
	memcpy(paquete->buffer->stream + paquete->buffer->size, drop->nombre_tabla, tamanio_nombre_tabla);
	paquete->buffer->size += tamanio_nombre_tabla;
	memcpy(paquete->buffer->stream + paquete->buffer->size, &drop->timestamp, sizeof(drop->timestamp));
	paquete->buffer->size += sizeof(drop->timestamp);
	free(drop->nombre_tabla);
	free(drop);
}

void empaquetar_journal(t_paquete * paquete, Journal * journal) {
	paquete->buffer->stream = malloc(sizeof(journal->timestamp));
	memcpy(paquete->buffer->stream, &journal->timestamp, sizeof(journal->timestamp));
	paquete->buffer->size += sizeof(journal->timestamp);
	free(journal);
}

void empaquetar_gossip(t_paquete * paquete, Gossip * gossip) {
	int cantidad_memorias = list_size(gossip->lista_memorias);
	if (cantidad_memorias == 0){
		list_destroy(gossip->lista_memorias);
		free(gossip);
		return;
	}
	paquete->buffer->stream = malloc(sizeof(int));
	memcpy(paquete->buffer->stream, &cantidad_memorias, sizeof(int));
	paquete->buffer->size += sizeof(int);
	Memoria *memoria;
	while(cantidad_memorias > 0){
		memoria = list_get(gossip->lista_memorias, cantidad_memorias - 1);
		size_t tamanio_ip = (strlen(memoria->ip) + 1 );
		size_t tamanio_puerto = (strlen(memoria->puerto) + 1 );
		size_t tamanio_id = sizeof(memoria->idMemoria);
		size_t tamanio = tamanio_ip + sizeof(size_t) + tamanio_puerto + sizeof(size_t) + tamanio_id;
		paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
		memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_ip, sizeof(size_t));
		paquete->buffer->size += sizeof(size_t);
		memcpy(paquete->buffer->stream + paquete->buffer->size, memoria->ip, (strlen(memoria->ip)+1));
		paquete->buffer->size += strlen(memoria->ip) + 1;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_puerto, sizeof(size_t));
		paquete->buffer->size += sizeof(size_t);
		memcpy(paquete->buffer->stream + paquete->buffer->size, memoria->puerto, (strlen(memoria->puerto)+1));
		paquete->buffer->size += strlen(memoria->puerto) + 1;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &memoria->idMemoria, sizeof(memoria->idMemoria));
		paquete->buffer->size += sizeof(memoria->idMemoria);
		cantidad_memorias--;
	}
	list_destroy_and_destroy_elements(gossip->lista_memorias, (void*)eliminar_memoria);
	free(gossip);
}

Select *desempaquetar_select(void* stream) {
	int desplazamiento = 0;
	Select *select = malloc(sizeof(Select));
	int tamanio;
	memcpy(&select->key, stream, sizeof(select->key));
	desplazamiento += sizeof(select->key);
	memcpy(&tamanio, stream + desplazamiento, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	select->nombre_tabla = malloc(tamanio);
	memcpy(select->nombre_tabla, stream + desplazamiento, tamanio);
	desplazamiento += tamanio;
	memcpy(&select->timestamp, stream + desplazamiento, sizeof(select->timestamp));
	return select;
}

Insert *desempaquetar_insert(void* stream) {
	int desplazamiento = 0;
	Insert *insert = malloc(sizeof(Insert));
	size_t tamanio;
	memcpy(&insert->key, stream, sizeof(insert->key));
	desplazamiento += sizeof(insert->key);
	memcpy(&tamanio, stream + desplazamiento, sizeof(tamanio));
	desplazamiento += sizeof(tamanio);
	insert->nombre_tabla = malloc(tamanio);
	memcpy(insert->nombre_tabla, stream + desplazamiento, tamanio);
	desplazamiento += tamanio;
	memcpy(&insert->timestamp, stream + desplazamiento, sizeof(insert->timestamp));
	desplazamiento += sizeof(insert->timestamp);
	memcpy(&insert->timestamp_insert, stream + desplazamiento, sizeof(insert->timestamp_insert));
	desplazamiento += sizeof(insert->timestamp_insert);
	memcpy(&tamanio, stream + desplazamiento, sizeof(tamanio));
	desplazamiento += sizeof(tamanio);
	insert->value = malloc(tamanio);
	memcpy(insert->value, stream + desplazamiento, tamanio);
	return insert;
}

Create *desempaquetar_create(void* stream) {
	int desplazamiento = 0;
	Create *create = malloc(sizeof(Create));
	size_t tamanio;
	memcpy(&create->compactation_time, stream, sizeof(create->compactation_time));
	desplazamiento += sizeof(create->compactation_time);
	memcpy(&create->consistencia, stream + desplazamiento, sizeof(create->consistencia));
	desplazamiento += sizeof(create->consistencia);
	memcpy(&tamanio, stream + desplazamiento, sizeof(tamanio));
	desplazamiento += sizeof(tamanio);
	create->nombre_tabla = malloc(tamanio);
	memcpy(create->nombre_tabla, stream + desplazamiento, tamanio);
	desplazamiento += tamanio;
	memcpy(&create->particiones, stream + desplazamiento, sizeof(create->particiones));
	desplazamiento += sizeof(create->particiones);
	memcpy(&create->timestamp, stream + desplazamiento, sizeof(create->timestamp));
	return create;
}
Describe *desempaquetar_describe(void* stream) {
	int desplazamiento = 0;
	Describe *describe = malloc(sizeof(Describe));
	size_t tamanio;
	memcpy(&tamanio, stream, sizeof(tamanio));
	desplazamiento += sizeof(tamanio);
	describe->nombre_tabla = malloc(tamanio);
	memcpy(describe->nombre_tabla, stream + desplazamiento, tamanio);
	desplazamiento += tamanio;
	memcpy(&describe->timestamp, stream + desplazamiento, sizeof(describe->timestamp));
	return describe;
}

Drop *desempaquetar_drop(void* stream) {
	int desplazamiento = 0;
	Drop *drop = malloc(sizeof(Drop));
	size_t tamanio;
	memcpy(&tamanio, stream, sizeof(tamanio));
	desplazamiento += sizeof(tamanio);
	drop->nombre_tabla = malloc(tamanio);
	memcpy(drop->nombre_tabla, stream + desplazamiento, tamanio);
	desplazamiento += tamanio;
	memcpy(&drop->timestamp, stream + desplazamiento, sizeof(drop->timestamp));
	return drop;
}
Journal *desempaquetar_journal(void* stream) {
	Journal *journal = malloc(sizeof(Journal));
	memcpy(&journal->timestamp, stream, sizeof(journal->timestamp));
	return journal;
}

Gossip *desempaquetar_gossip(void* stream){
	int desplazamiento = 0;
	Gossip *gossip = malloc(sizeof(Gossip));
	gossip->lista_memorias = list_create();
	size_t cantidad_memorias, tamanio;
	memcpy(&cantidad_memorias, stream, sizeof(cantidad_memorias));
	desplazamiento += sizeof(cantidad_memorias);
	while(cantidad_memorias != 0){
		Memoria *memoria = malloc(sizeof(Memoria));
		memcpy(&tamanio, stream + desplazamiento, sizeof(tamanio));
		desplazamiento += sizeof(tamanio);
		memoria->ip = malloc(tamanio);
		memcpy(memoria->ip, stream + desplazamiento, tamanio);
		desplazamiento += tamanio;
		memcpy(&tamanio, stream + desplazamiento, sizeof(tamanio));
		desplazamiento += sizeof(tamanio);
		memoria->puerto = malloc(tamanio);
		memcpy(memoria->puerto, stream + desplazamiento, tamanio);
		desplazamiento += tamanio;
		memcpy(&memoria->idMemoria, stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		list_add(gossip->lista_memorias, memoria);
		cantidad_memorias--;
	}
	return gossip;
}

bool validar_sender(Procesos sender, Procesos receiver, Tipo_Comunicacion comunicacion){
	switch (comunicacion){
	case T_INSTRUCCION:
		switch (receiver){
		case KERNEL:
			return false;
		case POOLMEMORY:
			return sender == KERNEL;
		case FILESYSTEM:
			return sender == POOLMEMORY;
		}
	case T_GOSSIPING:
		return ((receiver == POOLMEMORY) && (sender == KERNEL || sender == POOLMEMORY));
	case T_VALUE:
		return sender == POOLMEMORY && receiver == FILESYSTEM;
	default:
		return false;
	}
}

Instruccion *responder(int fd_a_responder, Instruccion *instruccion){
	t_paquete_retorno *paquete = crear_paquete_retorno(instruccion);
	if(enviar_paquete_retorno(paquete, fd_a_responder)){
		fsync(fd_a_responder);
		eliminar_paquete_retorno(paquete);
		return respuesta_success();
	}else{
		fsync(fd_a_responder);
		eliminar_paquete_retorno(paquete);
		return respuesta_error(CONNECTION_ERROR);
	}
}

Instruccion *recibir_respuesta(int fd_a_escuchar){
	Instruction_set retorno;
	if((recv(fd_a_escuchar, &retorno, sizeof(Instruction_set), MSG_WAITALL)) <= 0){
		return respuesta_error(CONNECTION_ERROR);
	}
	switch(retorno){
		case RETORNO:
			return recibir_retorno(fd_a_escuchar);
			break;
		default:
			return recibir_error(fd_a_escuchar);
			break;
	}
}

Instruccion *recibir_error(int fd_a_escuchar){
	Error_set tipo_error = UNKNOWN;
	size_t buffer_size;
	if ((recv(fd_a_escuchar, &buffer_size, sizeof(buffer_size), MSG_WAITALL)) <= 0){

		return respuesta_error(CONNECTION_ERROR);
	}
	if ((recv(fd_a_escuchar, &tipo_error, buffer_size, MSG_WAITALL)) <= 0){

		return respuesta_error(CONNECTION_ERROR);
	}else{
		return respuesta_error(tipo_error);
	}
}


Instruccion *recibir_retorno(int fd_a_escuchar){
	size_t buffer_size;
	if ((recv(fd_a_escuchar, &buffer_size, sizeof(buffer_size), MSG_WAITALL)) <= 0){
		return respuesta_error(CONNECTION_ERROR);
	}
	void *stream = malloc(buffer_size);
	if ((recv(fd_a_escuchar, stream, buffer_size, MSG_WAITALL)) <= 0){
		free(stream);
		return respuesta_error(CONNECTION_ERROR);
	}
	Tipo_Retorno tipo_ret;
	memcpy(&tipo_ret, stream, sizeof(Tipo_Retorno));
	void *chunk = stream + sizeof(Tipo_Retorno);
	Instruccion * inst;
	switch(tipo_ret){
		case VALOR:;
			inst = armar_retorno_value(chunk);
			break;
		case DATOS_DESCRIBE:;
			inst = armar_retorno_describe(chunk);
			break;
		case TAMANIO_VALOR_MAXIMO:;
			inst = armar_retorno_max_value(chunk);
			break;
		case SUCCESS:;
			inst = respuesta_success();
			break;
		case RETORNO_GOSSIP:;
			inst = armar_retorno_gossip(chunk);
			break;
		default:
			inst = respuesta_error(UNKNOWN);
			break;
	}
	free(stream);
	return inst;
}

Instruccion *respuesta_error(Error_set error){
	Instruccion *respuesta = malloc(sizeof(Instruccion));
	Error * error_a_responder = malloc(sizeof(Error));
	error_a_responder->error = error;
	respuesta->instruccion = ERROR;
	respuesta->instruccion_a_realizar = error_a_responder;
	return respuesta;
}

Instruccion *respuesta_success(void){
	Instruccion *respuesta_success = malloc(sizeof(Instruccion));
	Retorno_Generico *success = malloc(sizeof(Retorno_Generico));
	success->tipo_retorno = SUCCESS;
	respuesta_success->instruccion = RETORNO;
	respuesta_success->instruccion_a_realizar = success;
	return respuesta_success;
}

Instruccion *armar_retorno_value(void *chunk){
	int desplazamiento = 0;
	size_t tamanio_value;
	Retorno_Value *ret_val = malloc(sizeof(Retorno_Value));
	memcpy(&tamanio_value, chunk + desplazamiento, sizeof(tamanio_value));
	desplazamiento += sizeof(tamanio_value);
	ret_val->value = malloc(tamanio_value);
	memcpy(ret_val->value, chunk + desplazamiento, tamanio_value);
	desplazamiento += tamanio_value;
	memcpy(&ret_val->timestamp, chunk + desplazamiento, sizeof(t_timestamp));
	desplazamiento += sizeof(t_timestamp);
	Instruccion *instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion = RETORNO;
	Retorno_Generico *retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno = VALOR;
	retorno->retorno = ret_val;
	instruccion->instruccion_a_realizar = retorno;
	return instruccion;
}

Instruccion *armar_retorno_max_value(void * chunk){
	Retorno_Max_Value *ret_max_value = malloc(sizeof(Retorno_Max_Value));
	memcpy(&ret_max_value->value_size, chunk, sizeof(ret_max_value->value_size));
	Instruccion *instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion = RETORNO;
	Retorno_Generico *retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno = TAMANIO_VALOR_MAXIMO;
	retorno->retorno = ret_max_value;
	instruccion->instruccion_a_realizar = retorno;
	return instruccion;
}

Instruccion *armar_retorno_describe(void *chunk){
	int desplazamiento = 0;
	size_t cantidad_describes, valor;
	memcpy(&cantidad_describes, chunk, sizeof(size_t));
	desplazamiento += sizeof(int);
	Describes *describes = malloc(sizeof(Describes));
	describes->lista_describes = list_create();
	while(cantidad_describes > 0){
		Retorno_Describe *ret_desc = malloc(sizeof(Retorno_Describe));
		memcpy(&valor, chunk + desplazamiento, sizeof(size_t));
		desplazamiento += sizeof(size_t);
		ret_desc->nombre_tabla = malloc(valor);
		memcpy(ret_desc->nombre_tabla, chunk + desplazamiento, valor);
		desplazamiento += valor;
		memcpy(&ret_desc->consistencia, chunk + desplazamiento, sizeof(ret_desc->consistencia));
		desplazamiento += sizeof(ret_desc->consistencia);
		memcpy(&ret_desc->particiones, chunk + desplazamiento, sizeof(ret_desc->particiones));
		desplazamiento += sizeof(ret_desc->particiones);
		memcpy(&ret_desc->compactation_time, chunk + desplazamiento, sizeof(ret_desc->compactation_time));
		desplazamiento += sizeof(ret_desc->compactation_time);
		list_add(describes->lista_describes, ret_desc);
		cantidad_describes--;
	}
	Instruccion *instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion = RETORNO;
	Retorno_Generico *retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno = DATOS_DESCRIBE;
	retorno->retorno = describes;
	instruccion->instruccion_a_realizar = retorno;
	return instruccion;
}

Instruccion *armar_retorno_gossip(void *chunk){
	Instruccion *instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion = RETORNO;
	Retorno_Generico *retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno = RETORNO_GOSSIP;
	retorno->retorno = desempaquetar_gossip(chunk);
	instruccion->instruccion_a_realizar = retorno;
	return instruccion;
}

void empaquetar_retorno_valor(t_paquete_retorno *paquete, Retorno_Value *ret_val){
	Tipo_Retorno header = VALOR;
	size_t tamanio_value = strlen(ret_val->value) + 1;
	paquete->buffer->stream = malloc(sizeof(header) + sizeof(tamanio_value) + tamanio_value + sizeof(ret_val->timestamp));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &header, sizeof(header));
	paquete->buffer->size += sizeof(header);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_value, sizeof(tamanio_value));
	paquete->buffer->size += sizeof(tamanio_value);
	memcpy(paquete->buffer->stream + paquete->buffer->size, ret_val->value, tamanio_value);
	paquete->buffer->size += tamanio_value;
	memcpy(paquete->buffer->stream + paquete->buffer->size, &ret_val->timestamp, sizeof(ret_val->timestamp));
	paquete->buffer->size += sizeof(ret_val->timestamp);
	free(ret_val->value);
	free(ret_val);
}

void empaquetar_retorno_describe(t_paquete_retorno *paquete, Describes *describes){
	Tipo_Retorno header = DATOS_DESCRIBE;
	size_t cantidad_describes = list_size(describes->lista_describes);
	paquete->buffer->stream = malloc(sizeof(header) + sizeof(cantidad_describes));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &header, sizeof(header));
	paquete->buffer->size += sizeof(header);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &cantidad_describes, sizeof(cantidad_describes));
	paquete->buffer->size += sizeof(cantidad_describes);
	while(cantidad_describes > 0){
		Retorno_Describe *ret_desc;
		ret_desc = list_get(describes->lista_describes, cantidad_describes-1);
		size_t tamanio_nombre_tabla = strlen(ret_desc->nombre_tabla) + 1;
		size_t tamanio = sizeof(tamanio_nombre_tabla) + tamanio_nombre_tabla + sizeof(ret_desc->consistencia) + sizeof(ret_desc->particiones) + sizeof(ret_desc->compactation_time);
		paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
		memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_nombre_tabla, sizeof(tamanio_nombre_tabla));
		paquete->buffer->size += sizeof(tamanio_nombre_tabla);
		memcpy(paquete->buffer->stream + paquete->buffer->size, ret_desc->nombre_tabla, tamanio_nombre_tabla);
		paquete->buffer->size += tamanio_nombre_tabla;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &ret_desc->consistencia, sizeof(ret_desc->consistencia));
		paquete->buffer->size += sizeof(ret_desc->consistencia);
		memcpy(paquete->buffer->stream + paquete->buffer->size, &ret_desc->particiones, sizeof(ret_desc->particiones));
		paquete->buffer->size += sizeof(ret_desc->particiones);
		memcpy(paquete->buffer->stream + paquete->buffer->size, &ret_desc->compactation_time, sizeof(ret_desc->compactation_time));
		paquete->buffer->size += sizeof(ret_desc->compactation_time);
		cantidad_describes--;
	}
	list_destroy_and_destroy_elements(describes->lista_describes, (void*)eliminar_describe);
	free(describes);
}

void empaquetar_retorno_max_val(t_paquete_retorno *paquete, Retorno_Max_Value *max_val){
	Tipo_Retorno header = TAMANIO_VALOR_MAXIMO;
	paquete->buffer->stream = malloc(sizeof(header) + sizeof(max_val->value_size));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &header, sizeof(header));
	paquete->buffer->size += sizeof(header);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &max_val->value_size, sizeof(max_val->value_size));
	paquete->buffer->size += sizeof(max_val->value_size);
	free(max_val);
}

void empaquetar_retorno_success(t_paquete_retorno *paquete){
	Tipo_Retorno header = SUCCESS;
	paquete->buffer->stream = malloc(sizeof(header));
	memcpy(paquete->buffer->stream, &header, sizeof(header));
	paquete->buffer->size += sizeof(header);
}

void empaquetar_retorno_gossip(t_paquete_retorno *paquete, Gossip* gossip){
	int cantidad_memorias = list_size(gossip->lista_memorias);
	Tipo_Retorno header = RETORNO_GOSSIP;
	paquete->buffer->stream = malloc(sizeof(header) + sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &header, sizeof(header));
	paquete->buffer->size += sizeof(header);
	memcpy(paquete->buffer->stream + paquete->buffer->size, &cantidad_memorias, sizeof(int));
	paquete->buffer->size += sizeof(int);
	while(cantidad_memorias > 0){
		Memoria *memoria = list_get(gossip->lista_memorias, cantidad_memorias - 1);
		size_t tamanio_ip = (strlen(memoria->ip) + 1 );
		size_t tamanio_puerto = (strlen(memoria->puerto) + 1 );
		size_t tamanio_id = sizeof(memoria->idMemoria);
		size_t tamanio = tamanio_ip + sizeof(size_t) + tamanio_puerto + sizeof(size_t) + tamanio_id;
		paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
		memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_ip, sizeof(size_t));
		paquete->buffer->size += sizeof(size_t);
		memcpy(paquete->buffer->stream + paquete->buffer->size, memoria->ip, (strlen(memoria->ip)+1));
		paquete->buffer->size += strlen(memoria->ip) + 1;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio_puerto, sizeof(size_t));
		paquete->buffer->size += sizeof(size_t);
		memcpy(paquete->buffer->stream + paquete->buffer->size, memoria->puerto, (strlen(memoria->puerto)+1));
		paquete->buffer->size += strlen(memoria->puerto) + 1;
		memcpy(paquete->buffer->stream + paquete->buffer->size, &memoria->idMemoria, sizeof(memoria->idMemoria));
		paquete->buffer->size += sizeof(memoria->idMemoria);
		cantidad_memorias--;
	}
	list_destroy_and_destroy_elements(gossip->lista_memorias, (void*)eliminar_memoria);
	free(gossip);
}

void empaquetar_retorno_error(t_paquete_retorno *paquete, Error *error){
	paquete->buffer->stream = malloc(sizeof(error->error));
	memcpy(paquete->buffer->stream, &error->error, sizeof(error->error));
	paquete->buffer->size += sizeof(error->error);
}

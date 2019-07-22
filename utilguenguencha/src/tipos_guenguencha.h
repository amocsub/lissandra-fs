#ifndef UTILGUENGUENCHA_TIPOS_GUENGUENCHA_H_
#define UTILGUENGUENCHA_TIPOS_GUENGUENCHA_H_

#include <stdint.h>
#include <stdio.h>
#include <commons/collections/list.h>

//nuestros tipos de datos
#define t_key uint16_t
#define t_timestamp uint32_t
#define t_flag bool

typedef enum {
	EC, SC, SHC, DISP, NOT_FOUND
} Consistencias;

typedef enum {
	SELECT, INSERT, CREATE, DESCRIBE, DROP, ADD, RUN, JOURNAL, METRICS, ERROR, GOSSIP, MAX_VALUE, RETORNO
} Instruction_set;

typedef enum {
	BAD_RESPONSE, BAD_KEY, MISSING_TABLE, TABLE_EXIST, UNKNOWN, BAD_REQUEST, MISSING_FILE, CONNECTION_ERROR, MEMORY_FULL, LARGE_VALUE,
	INSERT_FAILURE, NULL_REQUEST, BAD_OPERATION, BAD_PARTITION, BAD_COMPACTATION, BAD_CONSISTENCY, BAD_MEMORY, JOURNAL_FAILURE, 
	FILE_DELETE_ERROR, FILE_OPEN_ERROR, FILE_SYNC_ERROR, DIR_OPEN_ERROR, DIR_DELETE_ERROR, DIR_CREATE_ERROR, BLOCK_ASSIGN_ERROR, BLOCK_MAX_REACHED,
	DIV_BY_ZERO
} Error_set;

typedef enum {
	KERNEL, FILESYSTEM, POOLMEMORY
} Procesos;

typedef enum {
	T_GOSSIPING, T_INSTRUCCION, T_VALUE
} Tipo_Comunicacion;

typedef enum {
	VALOR, DATOS_DESCRIBE, TAMANIO_VALOR_MAXIMO, SUCCESS, RETORNO_GOSSIP
} Tipo_Retorno;

typedef struct {
	char* puerto_servidor;
	Procesos proceso;
} Comunicacion;

typedef struct{
	char* nombre_tabla;
	t_key key;
	t_timestamp timestamp;
} Select;

typedef struct {
	char* nombre_tabla;
	t_key key;
	char* value;
	t_timestamp timestamp_insert;
	t_timestamp timestamp;
} Insert;

typedef struct {
	char* nombre_tabla;
	Consistencias consistencia;
	uint8_t particiones;
	t_timestamp compactation_time;
	t_timestamp timestamp;
} Create;

typedef struct {
	char* nombre_tabla;
	t_timestamp timestamp;
} Describe;

typedef struct {
	char* nombre_tabla;
	t_timestamp timestamp;
} Drop;

typedef struct {
	uint8_t memoria;
	Consistencias consistencia;
	t_timestamp timestamp;
} Add;

typedef struct {
	char* path;
	t_timestamp timestamp;
} Run;

typedef struct {
	t_timestamp timestamp;
} Journal;

typedef struct {
	t_timestamp timestamp;
} Metrics;

typedef struct {
	Instruction_set instruccion;
	void * instruccion_a_realizar;
} Instruccion;

typedef struct{
	Error_set error;
} Error;

typedef struct {
	size_t size;
	void* stream;
} t_buffer;

typedef struct {
	Tipo_Comunicacion comunicacion;
	Procesos source;
	Instruction_set header;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	Instruction_set header;
	t_buffer* buffer;
} t_paquete_retorno;

typedef struct {
	char* ip;
	char* puerto;
	int idMemoria;
} Memoria;

typedef struct {
	t_list *lista_memorias;
} Gossip;

typedef struct {
	t_list *lista_describes;
} Describes;

typedef struct {
	char *value;
	t_timestamp timestamp;
} Retorno_Value;

typedef struct {
	char *nombre_tabla;
	Consistencias consistencia;
	uint8_t particiones;
	t_timestamp compactation_time;
} Retorno_Describe;

typedef struct {
	size_t value_size;
} Retorno_Max_Value;

typedef struct{
	Tipo_Retorno tipo_retorno;
	void * retorno;
} Retorno_Generico;


#endif /* UTILGUENGUENCHA_TIPOS_GUENGUENCHA_H_ */

#ifndef UTILGUENGUENCHA_PARSER_H_
#define UTILGUENGUENCHA_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include "comunicacion.h"
#include "utils.h"

/**
* @NAME: parser_lql
* @DESC: Convierte un char* en un struct de tipo Instruccion
*
* Recibe el char* a parsear y el enum del proceso que lo esta enviando.
*/
Instruccion* parser_lql(char*, Procesos);

/**
* @NAME: cantidad_elementos
* @DESC: Dado un array con ultimo elemento NULL, devuelve la cantidad de elementos que contiene.
*/
int cantidad_elementos(char**);

/**
* @NAME: es_numero
* @DESC: Dado un string devuelve true si es un numero.
*/
bool es_numero(char*);

/**
* @NAME: print_instruccion_parseada
* @DESC: Imprime por pantalla la instruccion pasada como parametro, no libera la memoria.
*/
void print_instruccion_parseada(Instruccion*);

/**
* @NAME: log_instruccion_parseada
* @DESC: Loguea sin visualizacion la instruccion pasada como parametro y libera la memoria.
*/
void log_instruccion_parseada(Instruccion *);

/**
* @NAME: crear_instruccion
* @DESC: Crea una instruccion pasandole como parametro el enum de la operacion, el puntero a una operacion y el sizeof() de esa operacion.
*/
Instruccion* crear_instruccion(Instruction_set, void* instruccion_a_realizar);

/**
* @NAME: es_select
* @DESC: Devuelve true si el primer elemento del array es SELECT.
*/
bool es_select(char**);

/**
* @NAME: es_insert
* @DESC: Devuelve true si el primer elemento del array es INSERT.
*/
bool es_insert(char**);

/**
* @NAME: es_create
* @DESC: Devuelve true si el primer elemento del array es CREATE.
*/
bool es_create(char**);

/**
* @NAME: es_describe
* @DESC: Devuelve true si el primer elemento del array es DESCRIBE.
*/
bool es_describe(char**);

/**
* @NAME: es_drop
* @DESC: Devuelve true si el primer elemento del array es DROP.
*/
bool es_drop(char**);

/**
* @NAME: es_add
* @DESC: Devuelve true si el primer elemento del array es ADD.
*/
bool es_add(char**);

/**
* @NAME: es_run
* @DESC: Devuelve true si el primer elemento del array es RUN.
*/
bool es_run(char**);

/**
* @NAME: es_metrics
* @DESC: Devuelve true si el primer elemento del array es SELECT.
*/
bool es_metrics(char**);

/**
* @NAME: es_journal
* @DESC: Devuelve true si el primer elemento del array es SELECT.
*/
bool es_journal(char**);

/**
* @NAME: es_error
* @DESC: Devuelve true si el primer elemento del array es SELECT.
*/
bool es_error(char**);

/**
* @NAME: string_to_ulint
* @DESC: Convierte un char* en long int.
*/
unsigned long int string_to_ulint(char*);

/**
* @NAME: leer_por_consola
* @DESC: Readline que luego de leer aplica f sobre lo leido.
*/
void leer_por_consola(void (*f) (char*));

/**
* @NAME: get_timestamp
* @DESC: Devuelve el timestamp del sistema.
*/
t_timestamp get_timestamp();

/**
* @NAME: free_retorno
* @DESC: Libera una instruccion tipo RETORNO.
*/
void free_retorno(Instruccion * instruccion_parseada);

#endif /* UTILGUENGUENCHA_PARSER_H_ */

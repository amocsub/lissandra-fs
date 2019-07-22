#ifndef POOLMEMORY_H_
#define POOLMEMORY_H_

#include "../../utilguenguencha/src/comunicacion.h"
#include "../../utilguenguencha/src/parser.h"
#include "../../utilguenguencha/src/utils.h"


// Variables globales del proceso

char* PUERTO_DE_ESCUCHA;
char* IP_FS;
char* PUERTO_FS;
char** IP_SEEDS;
char** PUERTOS_SEEDS;
uint32_t RETARDO_MEM;
uint32_t RETARDO_FS;
uint32_t RETARDO_JOURNAL;
uint32_t RETARDO_GOSSIPING;

int SIZE_MEM;
int NUMERO_MEMORIA;
int MAX_VAL;

void* MEMORIA_PRINCIPAL; //puntero a malloc gigante
//int PAGINAS_MODIFICADAS; //contador de paginas modificadas, para simplificar el memory_full
//int PAGINAS_USADAS;		 //contador de paginas en uso
t_list* L_MARCOS;		 //lista de "marcos" de la memoria
t_list* L_SEGMENTOS;	 //lista de segmentos, cada segmento tiene su lista de paginas
t_list* L_MEMORIAS;
t_list* L_SEEDS;


pthread_mutex_t mutexMarcos, mutexSegmentos, mutexMemorias, mutexListaGossip;
pthread_rwlock_t lock_journal;

//  estructuras
typedef struct{
	char* nombre;
	t_list* paginas;
}Segmento;

typedef struct{
	void* pagina;
	t_timestamp ultimo_uso;
	t_flag en_uso;
}Marco;


// Funciones del proceso
void configuracion_inicial();
void retorno_consola(char* leido);
void inicializar_memoria(void);
Instruccion* atender_consulta (Instruccion*);
int insertar_en_memoria(char*, t_key, char*, t_timestamp, t_flag);
void* get_pagina(int id_pagina);
void agregar_pagina_en_segmento(Segmento*, int);
void* buscar_segmento(char*);
int index_segmento(char*);
int buscar_pagina_en_segmento(Segmento*, t_key);
bool coincide_segmento (char*, Segmento*);
bool coincide_pagina (t_key, int);
void eliminar_de_memoria(char*);  //elimina una tabla de memoria
t_timestamp* get_timestamp_pagina( void*);
t_key* get_key_pagina( void*);
char* get_value_pagina( void*);
t_flag* get_modificado_pagina( void*);
void set_timestamp_pagina( void*, t_timestamp);
void set_key_pagina( void*, t_key);
void set_value_pagina( void*, char*);
void set_modificado_pagina( void*, t_flag);
void print_lista_paginas();
void print_pagina(void*);
void lanzar_gossiping(void);
void* f_journaling(void);
int lanzar_journal(t_timestamp);
void* pedir_pagina(void);
int seleccionar_marco(void);
Segmento* crear_segmento(char*);
bool pagina_en_uso(Marco*);
bool memoria_full(void);
Instruccion* crear_error(Error_set);
void lanzar_gossiping(void);
void gossipear(Memoria *mem);
void add_memory_if_not_exists(Memoria *mem);
bool existe_memoria(t_list *lista, Memoria *mem1);
void sacar_memoria(Memoria *mem);
int posicion_de_memoria(t_list *lista, Memoria *mem);
void *TH_confMonitor(void * p);
void marcar_ultimo_uso(int id_pagina);
int marco_por_LRU(void);
void eliminar_referencia(int id_pagina);
t_list* filtrar_memorias_gossipear(void);
t_list *filtrar_memorias_a_enviar(void);
int count_paginas_modificadas(void);
int count_paginas_usadas(void);
void print_memory(t_log * LOGGER);

#endif

#ifndef UTILGUENGUENCHA_UTILS_H_
#define UTILGUENGUENCHA_UTILS_H_

#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "tipos_guenguencha.h"

#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024*(EVENT_SIZE + 16))
t_log * LOG_INFO;
t_log * LOG_ERROR;
t_log * LOG_DEBUG;
t_log * LOG_OUTPUT;
t_log * LOG_ERROR_SV;
t_log * LOG_OUTPUT_SV;
char* PATH_CONFIG;
t_dictionary *fd_disponibles;
t_list *fd_desafectados;
void configure_logger(void);
void exit_gracefully(int error);
char *consistencia2string(Consistencias consistencia);
int string2consistencia(char* consistencia);
int monitorNode(char * node,int mode,int(*callback)(void));
void eliminar_y_cerrar_fd_abiertos(int * fd);
void print_guenguencha(char* quien_soy);
char LOCAL_IP[16];
void handler(int s);
char *get_local_ip(void);
Memoria *duplicar_memoria(Memoria *memoria);
Retorno_Describe *duplicar_describe(Retorno_Describe *describe);
t_list * list_duplicate_all(t_list *lista, void*(*duplicador)(void*), pthread_mutex_t mutex);

/**
* @NAME: elimina_memoria()
* @DESC: para eliminar las memorias de una lista de gossip
*/
void eliminar_memoria(Memoria * memoria);
/**
* @NAME: eliminar_describe()
* @DESC: para eliminar describe de una lista de describes
*/
void eliminar_describe(Retorno_Describe *ret_desc);
/**
* @NAME: mostrar_memoria()
* @DESC: Mostrar las memorias
*/
void mostrar_memoria(Memoria * memoria);
/**
* @NAME: config_get_string_value_check()
* @DESC: Realiza el get string de un config, si no lo encuentra o es null loguea y hace exit_gracefully.
*/
char* config_get_string_value_check(t_config* config, char *key);
/**
* @NAME: config_get_int_value_check()
* @DESC: Realiza el get int de un config, si no lo encuentra o es null loguea y hace exit_gracefully.
*/
int config_get_int_value_check(t_config* config, char *key);


#endif /* UTILGUENGUENCHA_UTILS_H_ */

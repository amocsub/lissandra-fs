#include "kernel.h"

void configuracion_inicial(void) {

	t_config* CONFIG = config_create(PATH_CONFIG);

	if (!CONFIG) {
		log_error(LOG_ERROR, "No encuentro el archivo config");
		exit_gracefully(EXIT_FAILURE);
	}

	LOGGER_METRICS = log_create("log_metrics.log", "log_metrics", 0, LOG_LEVEL_INFO);
	
	//IP_MEMORIA_PPAL
	char * ipMemoriaPrincipal = config_get_string_value_check(CONFIG,"IP_MEMORIA_PPAL");
	IP_MEMORIA_PPAL = malloc(strlen(ipMemoriaPrincipal) + 1);
	strcpy(IP_MEMORIA_PPAL, ipMemoriaPrincipal);

	//PUERTO_MEMORIA_PPAL
	char * puertoMemoriaPrincipal = config_get_string_value_check(CONFIG,"PUERTO_MEMORIA_PPAL");
	PUERTO_MEMORIA_PPAL = malloc(strlen(puertoMemoriaPrincipal) + 1);
	strcpy(PUERTO_MEMORIA_PPAL, puertoMemoriaPrincipal);

	//QUANTUM
	QUANTUM = config_get_int_value_check(CONFIG, "QUANTUM");

	//TAMANO_MAXIMO_LECTURA_ARCHIVO
	TAMANO_MAXIMO_LECTURA_ARCHIVO = config_get_int_value_check(CONFIG, "TAMANO_MAXIMO_LECTURA_ARCHIVO");

	//MULTIPROCESAMIENTO
	HILOS_KERNEL = config_get_int_value_check(CONFIG, "MULTIPROCESAMIENTO");

	//TIEMPO_METRICS
	SEGUNDOS_METRICS = config_get_int_value_check(CONFIG, "TIEMPO_METRICS");

	//TIEMPO_PREGUNTAR_MEMORIA
	PREGUNTAR_POR_MEMORIAS = config_get_int_value_check(CONFIG, "TIEMPO_PREGUNTAR_MEMORIA");

	//TIEMPO_DESCRIBE
	TIEMPO_DESCRIBE = config_get_int_value_check(CONFIG, "TIEMPO_DESCRIBE");

	//RETARDO
	RETARDO = config_get_int_value_check(CONFIG, "RETARDO");

	config_destroy(CONFIG);

	acum30sMetrics = list_create();
	MEM_LOAD = list_create();
	READS =0;
	WRITES =0;
	WRITE_LAT =0;
	READ_LAT =0;
}

void actualizar_configuracion(t_config* conf) {
	SEGUNDOS_METRICS = config_get_int_value_check(conf, "TIEMPO_METRICS");
	PREGUNTAR_POR_MEMORIAS = config_get_int_value_check(conf, "TIEMPO_PREGUNTAR_MEMORIA");
	TIEMPO_DESCRIBE = config_get_int_value_check(conf, "TIEMPO_DESCRIBE");
	QUANTUM = config_get_int_value_check(conf, "QUANTUM");
	RETARDO = config_get_int_value_check(conf, "RETARDO");

	log_info(LOG_INFO,"Se ha actualizado el archivo de configuracion: SEGUNDOS_METRICS: %d, PREGUNTAR_POR_MEMORIAS: %d, TIEMPO_DESCRIBE: %d, QUANTUM: %d, RETARDO: %d", SEGUNDOS_METRICS, PREGUNTAR_POR_MEMORIAS, TIEMPO_DESCRIBE, QUANTUM, RETARDO);

}

void iniciarEstados() {
	estadoReady = list_create();
	estadoNew = list_create();
	estadoExit = list_create();
	list_clean(estadoReady);
	list_clean(estadoNew);
	list_clean(estadoExit);
}

void iniciarEstructurasAsociadas(){
	lista_ec = list_create();
	lista_sc = list_create();
	lista_shc = list_create();
	lista_disp = list_create();
	metrics = dictionary_create();

}

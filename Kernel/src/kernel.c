#include "kernel.h"

// Hay que definirla ya que no tiene definición en kernel y sino rompería, de todos modos no se usa
void retornarControl(Instruccion *instruccion, int socket_cliente){};

void *TH_describe(void *p);

int main(int argc, char* argv[])  {

	pthread_mutex_init(&mutexRecursosCompartidos, NULL);
	pthread_mutex_init(&lista_de_tablas_mx, NULL);
	sem_init(&semaforoSePuedePlanificar, 0, 0);
	sem_init(&semaforoNewToReady, 0, 0);
	sem_init(&semaforoFinalizar, 0, 0);

	configure_logger();

	if (argc == 1) {
		PATH_CONFIG = "config.cfg";
	} else if (argc == 2) {
		PATH_CONFIG = argv[1];
	} else {
		log_error(LOG_ERROR,"Solo se acepta un argumento (PATH CONFIG) o ninguno para tomar config.cfg.");
		exit(EXIT_FAILURE);
	}

	configuracion_inicial();

	iniciarEstados();
	iniciarEstructurasAsociadas();
	
	fd_disponibles = dictionary_create();
	fd_desafectados = list_create();
	
	pthread_t memoriasDisponibles, pasarNewToReady, finalizarProceso, calcularMetrics, T_confMonitor, T_describe;
	
	pthread_create(&finalizarProceso, NULL, (void*) finalizar_procesos, NULL);
	pthread_detach(finalizarProceso);

	pthread_create(&memoriasDisponibles, NULL, (void*) lanzar_gossiping, NULL);
	pthread_detach(memoriasDisponibles);

	pthread_create(&T_describe, NULL, TH_describe, NULL);
	pthread_detach(T_describe);

	pthread_create(&T_confMonitor, NULL, TH_confMonitor, NULL);
	pthread_detach(T_confMonitor);

	pthread_create(&pasarNewToReady, NULL, (void*) newToReady, NULL);
	pthread_detach(pasarNewToReady);

	pthread_create(&calcularMetrics, NULL, (void*) calculoMetrics, NULL);
	pthread_detach(calcularMetrics);

	int cantMultiprocesamiento = 0;
	while (cantMultiprocesamiento < HILOS_KERNEL) {
		pthread_t multiProcesamientoKernell;
		pthread_create(&multiProcesamientoKernell, NULL, (void*) ejecutar,
		NULL);
		pthread_detach(multiProcesamientoKernell);
		cantMultiprocesamiento++;
	}

	print_guenguencha("  KERNEL  ");
	leer_por_consola(retorno_consola);
}

void retorno_consola(char* leido) {
	if(strcmp(leido, "memorias") == 0){
		pthread_mutex_lock(&mutex_disp);
		list_iterate(lista_disp, (void*)mostrar_memoria);
		pthread_mutex_unlock(&mutex_disp);
		return;
	}
	Instruccion * instruccion = parser_lql(leido, KERNEL);
	if (instruccion->instruccion != ERROR) {
		Proceso * proceso = malloc(sizeof(Proceso));
		proceso->instruccion = instruccion;
		proceso->instruccionAProcesar = NULL;
		proceso->numeroInstruccion = 0;
		proceso->quantumProcesado = 0;
		encolar(estadoNew, proceso);
		sem_post(&semaforoNewToReady);
	} else {
		free_retorno(instruccion);
	}
}

void newToReady() {
	while (true) {
		sem_wait(&semaforoNewToReady);
		Proceso * proceso = desencolar(estadoNew);
		encolar(estadoReady, proceso);
		sem_post(&semaforoSePuedePlanificar);
	}
}

void ejecutar() {
	while (true) {
		sem_wait(&semaforoSePuedePlanificar);
		Proceso * proceso = desencolar(estadoReady);
		switch (proceso->instruccion->instruccion) {
			case RUN:;
				proceso->fin_proceso = false;
				log_info(LOG_INFO, "Ejecuto el script %s desde la linea %d ", ((Run*)proceso->instruccion->instruccion_a_realizar)->path, (proceso->numeroInstruccion));
				while(proceso->quantumProcesado <= QUANTUM && !proceso->fin_proceso){
					usleep(RETARDO*1000);
					logicaRun(proceso);
				}
				if(!proceso->fin_proceso){
					log_info(LOG_INFO, "Corto de hacer el proceso %s, en la linea %d", ((Run*)proceso->instruccion->instruccion_a_realizar)->path, (proceso->numeroInstruccion));
					proceso->quantumProcesado = 0;
					encolar(estadoReady, proceso);
					sem_post(&semaforoSePuedePlanificar);
				}else{
					log_info(LOG_INFO, "Termine de ejecutar el script %s, %d lineas terminadas", ((Run*)proceso->instruccion->instruccion_a_realizar)->path, (proceso->numeroInstruccion));
					free(((Run*)proceso->instruccion->instruccion_a_realizar)->path);
					free(proceso->instruccion->instruccion_a_realizar);
					free(proceso->instruccion);
					encolar(estadoExit, proceso);
					sem_post(&semaforoFinalizar);
				}
				break;

			case METRICS:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaMetrics(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case SELECT:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaSelect(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case INSERT:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaInsert(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case CREATE:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaCreate(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case DROP:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaDrop(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case ADD:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaAdd(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			case DESCRIBE:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaDescribe(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;

			default:;
				usleep(RETARDO*1000);
				proceso->instruccionAProcesar = proceso->instruccion;
				logicaJournal(proceso);
				encolar(estadoExit, proceso);
				sem_post(&semaforoFinalizar);
				break;
		}
	}
}

/*
 Manejo Monitoreo
 */
void *TH_confMonitor(void * p) {

	int confMonitor_cb(void) {
		t_config* conf = config_create(PATH_CONFIG);
		if (conf == NULL) {
			log_error(LOG_ERROR, "Archivo de configuracion: %s no encontrado", PATH_CONFIG);
			return 1;
		}
		actualizar_configuracion(conf);
		config_destroy(conf);
		return 0;
	}

	int retMon = monitorNode(PATH_CONFIG, IN_MODIFY, &confMonitor_cb);
	if (retMon != 0) {
		return (void*) 1;
	}
	return (void*) 0;
}

/*
 Manejo Describe
 */
void *TH_describe(void *p) {
	lista_de_tablas = list_create();
	while (true) {
		realizarDescribeGeneral();
		usleep(TIEMPO_DESCRIBE * 1000);
	}
	list_destroy(lista_de_tablas);
	pthread_mutex_destroy(&lista_de_tablas_mx);
}

void finalizar_procesos(void){
	while(true){
		sem_wait(&semaforoFinalizar);
		Proceso * proceso = desencolar(estadoExit);
		free(proceso);
	}
}

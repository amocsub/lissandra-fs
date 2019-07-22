#include "kernel.h"

void calculoMetrics(){
	int contadorInsert, contadorSelect, contadorSelectInsert, tiempoTotalSelect, tiempoTotalInsert;
	t_list* contador_por_memoria;
	AcumuladorMemoria * acumuladorMemoria;

	while(true){
		pthread_mutex_lock(&mutex_metrics);

		contadorInsert = 0;
		contadorSelect = 0;
		contadorSelectInsert = 0;
		tiempoTotalSelect = 0;
		tiempoTotalInsert = 0;
		contador_por_memoria = list_create();
		acumuladorMemoria = NULL;

		int elemento = (acum30sMetrics->elements_count -1);

		while (elemento >= 0){

			AcumMetrics* acumMetrics = list_remove(acum30sMetrics, elemento);

			if (acumMetrics->id_memoria != -1){
				acumuladorMemoria = (AcumuladorMemoria*) dameAcumulador(acumMetrics->id_memoria, contador_por_memoria);

				if(acumuladorMemoria == NULL){
					acumuladorMemoria = malloc(sizeof(AcumuladorMemoria));
					acumuladorMemoria->id_memoria = acumMetrics->id_memoria;
					acumuladorMemoria->cantidad_instrucciones = 0;
					acumuladorMemoria->cantidad_ins_sel = 0;

					list_add(contador_por_memoria, acumuladorMemoria);
				}


				switch(acumMetrics->instruccion){
				case SELECT:;
					contadorSelect ++;
					tiempoTotalSelect += acumMetrics->tiempo;
					contadorSelectInsert++;
					acumuladorMemoria->cantidad_ins_sel++;
					acumuladorMemoria->cantidad_instrucciones++;
					break;
				case INSERT:;
					contadorInsert++;
					tiempoTotalInsert += acumMetrics->tiempo;
					contadorSelectInsert++;
					acumuladorMemoria->cantidad_ins_sel++;
					acumuladorMemoria->cantidad_instrucciones++;
					break;
				default:;
					acumuladorMemoria->cantidad_instrucciones++;
					break;
				}
				free(acumMetrics);
			}

		elemento--;
		}

		READS = contadorSelect;

		if(contadorSelect>0){
			READ_LAT = tiempoTotalSelect/contadorSelect;
		} else {
			READ_LAT = 0;
		}

		WRITES = contadorInsert;

		if(contadorInsert>0){
			WRITE_LAT = tiempoTotalInsert/contadorInsert;
		} else {
			WRITE_LAT = 0;
		}

		list_destroy_and_destroy_elements(MEM_LOAD, (void*)free);

		/*//PARA ORDENAR LA LISTA Y QUE QUEDE MAS LINDO, NO LE GUSTA EL BOOL*

		bool* orderMemory (AcumuladorMemoria* mem1, AcumuladorMemoria* mem2){
			bool resultado = (mem1->id_memoria < mem2->id_memoria);//la ordeno al de mayor a menor para imprimirla de abajo hacia arriba.
			return  &resultado;
		}

		list_sort(contador_por_memoria, (bool*)orderMemory);
		*/

		MEM_LOAD = contador_por_memoria;

		pthread_mutex_unlock(&mutex_metrics);

		loguear_metrics(LOGGER_METRICS);

		sleep(SEGUNDOS_METRICS);
	}
}

void loguear_metrics(t_log* LOGGER){

	pthread_mutex_lock(&mutex_metrics);

	log_info(LOGGER, "Read Latency: %f", READ_LAT);
	log_info(LOGGER, "Write Latency: %f", WRITE_LAT);
	log_info(LOGGER, "Reads: %d", READS);
	log_info(LOGGER, "Writes: %d", WRITES);

	int elemento = (MEM_LOAD->elements_count -1);

	AcumuladorMemoria* acum;
	if (elemento < 0){
		log_info(LOGGER_METRICS, "Memory: - Memory Load: No hay valores acumulados.");
	}

	int ins_sel_totales = READS + WRITES;

	while (elemento >= 0) {
		acum = list_get(MEM_LOAD, elemento);
		log_info(LOGGER, "Memory: %d Memory Load: %d / %d", acum->id_memoria, acum->cantidad_ins_sel, ins_sel_totales);
		elemento--;
	}

	pthread_mutex_unlock(&mutex_metrics);
}

AcumuladorMemoria* dameAcumulador(int id_memoria, t_list* lista_acumuladores){
	int elemento = lista_acumuladores->elements_count -1;
	AcumuladorMemoria* acum;

	while(elemento >=0 ){
		acum = list_get(lista_acumuladores, elemento);
		if (acum->id_memoria == id_memoria){
			return acum;
		}
		elemento--;
	}
	return NULL;
}

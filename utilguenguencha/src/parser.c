#include "parser.h"
void free_consulta_separada(int length,char** consulta_separada);
void log_describes(Retorno_Describe *describe);
void show_describes(Retorno_Describe *describe);

Instruccion* parser_lql(char* consulta, Procesos procesoOrigen) {

	if (string_is_empty(consulta)) {
		return respuesta_error(NULL_REQUEST);
	}

	Instruccion* consultaParseada;

	uintmax_t echo_time = get_timestamp();

	char * palabra = malloc(strlen(consulta) +1);
	strcpy(palabra, consulta);
	char * palabra2 = string_from_format("%s %ju", palabra, (uintmax_t) echo_time);
	free(palabra);
	char** consulta_separada = string_split(palabra2, " ");
	free(palabra2);

	int length = cantidad_elementos(consulta_separada) - 1;

	if (es_select(consulta_separada)){

		if (length != 3) {
			log_error(LOG_ERROR, "La sintaxis correcta es > SELECT [NOMBRE_TABLA] [KEY], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else if (!es_numero(consulta_separada[2])
				|| string_to_ulint(consulta_separada[2]) > 65535) {
			log_error(LOG_ERROR, "La Key es incorrecta, debe ser menor a 65.535!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_KEY);
		}else{
			Select * nuevoSelect = malloc(sizeof(Select));

			string_to_upper(consulta_separada[1]);
			nuevoSelect->nombre_tabla = malloc(strlen(consulta_separada[1]) + 1);
			strcpy(nuevoSelect->nombre_tabla, consulta_separada[1]);
			uint16_t key = (int) string_to_ulint(consulta_separada[2]);
			nuevoSelect->key = key;	 								// cargo key
			uint32_t timestamp = (uint32_t) string_to_ulint(consulta_separada[3]);
			nuevoSelect->timestamp = timestamp;				// cargo timestamp

			consultaParseada = crear_instruccion(SELECT, nuevoSelect);

		}
	} else if (es_insert(consulta_separada)) {
		if (length < 4) {
			log_error(LOG_ERROR, "La sintaxis correcta es > INSERT [NOMBRE_TABLA] [KEY] ”[VALUE]” ?[TIMESTAMP], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		}
		if (length >= 5) {

			int largo_primer_palabra = string_length(consulta_separada[3]);

			if ((length >= 5) & string_starts_with(consulta_separada[3], "\"")
					& (!string_ends_with(consulta_separada[3], "\"")
							|| largo_primer_palabra == 1)) {

				int i = 0;

				while ((!string_ends_with(consulta_separada[3], "\"") || i == 0)
						& (i + 4 < length)) {
					char* palabra1 = malloc(strlen(consulta_separada[3])+1);
					strcpy(palabra1, consulta_separada[3]);
					free(consulta_separada[3]);
					char* palabra2 = malloc(strlen(consulta_separada[4])+1);
					strcpy(palabra2, consulta_separada[4]);
					free(consulta_separada[4]);
					consulta_separada[3] = string_from_format("%s %s", palabra1, palabra2);
					free(palabra1);
					free(palabra2);
					i++;
					consulta_separada[4] = consulta_separada[4 + i];
				}
				consulta_separada[5] = consulta_separada[5 + i];

				while ((5 + i) < length){
					i++;
					consulta_separada[6] = consulta_separada[6 + i];
				}

				length = length - i;
			}

			if (length > 5) {
				log_error(LOG_ERROR, "La sintaxis correcta es > INSERT [NOMBRE_TABLA] [KEY] ”[VALUE]” ?[TIMESTAMP], chinguengencha!");
				free_consulta_separada(length,consulta_separada);
				return respuesta_error(BAD_REQUEST);
			}
		}


		if ((length == 5) && !es_numero(consulta_separada[4])) {
			log_error(LOG_ERROR, "La sintaxis correcta es > INSERT [NOMBRE_TABLA] [KEY] ”[VALUE]” ?[TIMESTAMP], chinguengencha!");

			free_consulta_separada(length,consulta_separada);

			return respuesta_error(BAD_REQUEST);
		} else if (!es_numero(consulta_separada[2])
				|| string_to_ulint(consulta_separada[2]) > 65535) {
			log_error(LOG_ERROR, "Parser: La Key es incorrecta, debe ser menor a 65.535");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_KEY);
		} else {

			/* INSERT */

			if(string_starts_with(consulta_separada[3], "\"") & string_ends_with(consulta_separada[3], "\"") ){

				char* palabra = malloc(strlen(consulta_separada[3])+1);
				strcpy(palabra, consulta_separada[3]);
				free(consulta_separada[3]);

				consulta_separada[3] = string_substring(palabra, 1, (string_length(palabra)-2));
				free(palabra);

				//saco las comillas del value
			}

			Insert* nuevoInsert = malloc(sizeof(Insert));

			string_to_upper(consulta_separada[1]);
			nuevoInsert->nombre_tabla = malloc(strlen(consulta_separada[1]) + 1);
			strcpy(nuevoInsert->nombre_tabla, consulta_separada[1]);// cargo tabla
			uint16_t key = (int) string_to_ulint(consulta_separada[2]);
			nuevoInsert->key = key;					 				// cargo key
	
			nuevoInsert->value = malloc(strlen(consulta_separada[3])+1);
			strcpy(nuevoInsert->value,consulta_separada[3]); // cargo value

			if (length == 4) {			// no vino con timestamp en la consulta
				uint32_t timestamp = (uint32_t) string_to_ulint(
						consulta_separada[4]);
				nuevoInsert->timestamp_insert = timestamp;// cargo timestamp_insert
				nuevoInsert->timestamp = timestamp;			// cargo timestamp
			} else {						// vino con timestamp en la consulta
				uint32_t timestamp_insert = (uint32_t) string_to_ulint(
						consulta_separada[4]);
				nuevoInsert->timestamp_insert = timestamp_insert;// cargo timestamp_insert
				uint32_t timestamp = (uint32_t) string_to_ulint(
						consulta_separada[5]);
				nuevoInsert->timestamp = timestamp;			// cargo timestamp
			}


			consultaParseada = crear_instruccion(INSERT, nuevoInsert);

		}
	} else if (es_create(consulta_separada)) {
		if (length != 5) {
			log_error(LOG_ERROR, "La sintaxis correcta es > CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else if (!es_numero(consulta_separada[3])) {
			log_error(LOG_ERROR,"La cantidad de particiones debe ser un numero.");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_PARTITION);
		} else if (!es_numero(consulta_separada[4])) {
			log_error(LOG_ERROR, "Parser: El tiempo de compactacion debe ser un numero.");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_COMPACTATION);
		} else {

			// es CREATE
			Create * nuevoCreate = malloc(sizeof(Create));

			string_to_upper(consulta_separada[1]);
			nuevoCreate->nombre_tabla = malloc(strlen(consulta_separada[1]) + 1);
			strcpy(nuevoCreate->nombre_tabla, consulta_separada[1]); // cargo tabla

			Consistencias consistencia;

			if (string_equals_ignore_case(consulta_separada[2], "SC")) {
				consistencia = SC;
			} else if (string_equals_ignore_case(consulta_separada[2], "SHC")) {
				consistencia = SHC;

			} else if (string_equals_ignore_case(consulta_separada[2], "EC")) {
				consistencia = EC;

			} else {
				log_error(LOG_ERROR, "Los criterios de consistencia aceptados son [SC, SHC, EC], chinguengencha!");
				free_consulta_separada(length,consulta_separada);
				return respuesta_error(BAD_CONSISTENCY);
			};

			nuevoCreate->consistencia = consistencia;	// cargo consistencia
			uint8_t particiones = (int) string_to_ulint(consulta_separada[3]);
			nuevoCreate->particiones = particiones;			// cargo particiones
			uint32_t compactation_time = (uint32_t) string_to_ulint(
					consulta_separada[4]);
			nuevoCreate->compactation_time = compactation_time;	// cargo compactation_time
			uint32_t timestamp = (uint32_t) string_to_ulint(
					consulta_separada[5]);
			nuevoCreate->timestamp = timestamp;				// cargo timestamp


			consultaParseada = crear_instruccion(CREATE, nuevoCreate);

		}
	} else if (es_describe(consulta_separada)) {

		if (length > 2) {
			log_error(LOG_ERROR, "La sintaxis correcta es > DESCRIBE [NOMBRE_TABLA] o DESCRIBE, chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else {

			// es DESCRIBE
			Describe * nuevoDescribe = malloc(sizeof(Describe));

			if (length == 2) {

				string_to_upper(consulta_separada[1]);
				nuevoDescribe->nombre_tabla = malloc(strlen(consulta_separada[1]) + 1);
				strcpy(nuevoDescribe->nombre_tabla, consulta_separada[1]);// cargo tabla
				uint32_t timestamp = (uint32_t) string_to_ulint(consulta_separada[2]);
				nuevoDescribe->timestamp = timestamp;		// cargo timestamp

			} else {
				uint32_t timestamp = (uint32_t) string_to_ulint(consulta_separada[1]);
				nuevoDescribe->timestamp = timestamp;		// cargo timestamp
				nuevoDescribe->nombre_tabla = NULL;

			}


			consultaParseada = crear_instruccion(DESCRIBE, nuevoDescribe);

		}
	} else if (es_drop(consulta_separada)) {

		if (length != 2) {
			log_error(LOG_ERROR, "La sintaxis correcta es > DROP [NOMBRE_TABLA], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else {

			// es DROP
			Drop* nuevoDrop = malloc(sizeof(Drop));
			string_to_upper(consulta_separada[1]);
			nuevoDrop->nombre_tabla = malloc(strlen(consulta_separada[1]) + 1);
			strcpy(nuevoDrop->nombre_tabla, consulta_separada[1]); // cargo tabla
			uint32_t timestamp = (uint32_t) string_to_ulint(
					consulta_separada[2]);
			nuevoDrop->timestamp = timestamp;				// cargo timestamp


			consultaParseada = crear_instruccion(DROP, nuevoDrop);
		}
	} else if (es_add(consulta_separada) & (procesoOrigen == KERNEL)) {
		char* palabra1;
		if (length != 5) {
			log_error(LOG_ERROR, "La sintaxis correcta es > ADD MEMORY [NÚMERO] TO [CRITERIO], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else if (!es_numero(consulta_separada[2])) {
			log_error(LOG_ERROR, "La memoria debe ser un numero.");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_MEMORY);
		} else if ( (palabra1 = string_from_format("%s %s", consulta_separada[0], consulta_separada[1])) && !(string_equals_ignore_case(palabra1, "ADD MEMORY")& (string_equals_ignore_case(consulta_separada[3], "TO")))) {
				log_error(LOG_ERROR, "La sintaxis correcta es > ADD MEMORY [NÚMERO] TO [CRITERIO], chinguengencha!");
				free_consulta_separada(length,consulta_separada);
				free(palabra1);
				return respuesta_error(BAD_REQUEST);
		} else {
			free(palabra1);
			// es ADD MEMORY
			Add * nuevoAddMemory = malloc(sizeof(Add));

			nuevoAddMemory->memoria = (int) string_to_ulint(
					consulta_separada[2]);			// cargo memoria

			Consistencias consistencia;

			if (string_equals_ignore_case(consulta_separada[4], "SC")) {
				consistencia = SC;
			} else if (string_equals_ignore_case(consulta_separada[4], "SHC")) {
				consistencia = SHC;
			} else if (string_equals_ignore_case(consulta_separada[4], "EC")) {
				consistencia = EC;
			} else {
				log_error(LOG_ERROR, "Los criterios de consistencia aceptados son [SC, SHC, EC]");
				free_consulta_separada(length,consulta_separada);
				return respuesta_error(BAD_CONSISTENCY);
			}

			nuevoAddMemory->consistencia = consistencia;// cargo consistencia
			uint32_t timestamp = (uint32_t) string_to_ulint(
					consulta_separada[5]);
			nuevoAddMemory->timestamp = timestamp;			// cargo timestamp



			consultaParseada = crear_instruccion(ADD, nuevoAddMemory);
		}
	} else if (es_run(consulta_separada) & (procesoOrigen == KERNEL)) {

		if (length != 2) {
			log_error(LOG_ERROR, "La sintaxis correcta es > RUN [ARCHIVO], chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else {

			// es RUN
			Run * nuevoRun = malloc(sizeof(Run));
			nuevoRun->path = malloc(strlen(consulta_separada[1]) + 1);
			strcpy(nuevoRun->path, consulta_separada[1]); // cargo path
			uint32_t timestamp = (uint32_t) string_to_ulint(
					consulta_separada[2]);
			nuevoRun->timestamp = timestamp;				 // cargo timestamp


			consultaParseada = crear_instruccion(RUN, nuevoRun);
		}
	} else if (es_metrics(consulta_separada) & (procesoOrigen == KERNEL)) {

		if (length != 1) {
			log_error(LOG_ERROR, "La sintaxis correcta es > METRICS, chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else {

			// es METRICS
			Metrics* nuevoMetrics = malloc(sizeof(Metrics));

			uint32_t timestamp = (uint32_t) string_to_ulint(
					consulta_separada[1]);
			nuevoMetrics->timestamp = timestamp;			// cargo timestamp


			consultaParseada = crear_instruccion(METRICS, nuevoMetrics);
		}
	} else if (es_journal(consulta_separada)
			& (procesoOrigen == POOLMEMORY || procesoOrigen == KERNEL)) {

		if (length != 1) {
			log_error(LOG_ERROR, "La sintaxis correcta es > JOURNAL, chinguengencha!");
			free_consulta_separada(length,consulta_separada);
			return respuesta_error(BAD_REQUEST);
		} else {

			// es JOURNAL
			Journal* nuevoJournal = malloc(sizeof(Journal));

			uint32_t timestamp = (uint32_t) string_to_ulint(consulta_separada[1]);
			nuevoJournal->timestamp = timestamp;			// cargo timestamp


			consultaParseada = crear_instruccion(JOURNAL, nuevoJournal);
		}
	} else {

		switch (procesoOrigen) {
		case KERNEL:;
			log_error(LOG_ERROR, "Las operaciones disponibles son: [SELECT, INSERT, CREATE, DESCRIBE, DROP, ADD MEMORY, RUN, METRICS, JOURNAL]");
			break;

		case POOLMEMORY:;
			log_error(LOG_ERROR, "Las operaciones disponibles son: [SELECT, INSERT, CREATE, DESCRIBE, DROP, JOURNAL]");
			break;

		case FILESYSTEM:;
			log_error(LOG_ERROR, "Las operaciones disponibles son: [SELECT, INSERT, CREATE, DESCRIBE, DROP]");
			break;

		}

		free_consulta_separada(length,consulta_separada);
		return respuesta_error(BAD_OPERATION);
	}
	free_consulta_separada(length,consulta_separada);
	return consultaParseada;
}

Instruccion* crear_instruccion(Instruction_set operacion, void* instruccion_a_realizar) {
	Instruccion * p_instruccion = malloc(sizeof(Instruccion));
	p_instruccion->instruccion = operacion;
	p_instruccion->instruccion_a_realizar = instruccion_a_realizar;
	return p_instruccion;

}


int cantidad_elementos(char ** array) {
	int i = 0;

	while (array[i] != '\0') {
		i++;
	}

	return i;

}

bool es_numero(char* palabra) {
	int length = string_length(palabra);

	for (int i = 0; i < length; i++) {
		if (!isdigit(palabra[i])) {
			return false;
		}
	}

	return true;
}

void show_describes(Retorno_Describe *describe){
	char*consistencia=consistencia2string(describe->consistencia);
	log_info(LOG_OUTPUT,"Nombre tabla: %s Consistencia: %s Particiones: %d Tiempo de compactacion: %d", describe->nombre_tabla, consistencia, describe->particiones, describe->compactation_time);
	free(consistencia);
}

void log_describes(Retorno_Describe *describe){
	char*consistencia=consistencia2string(describe->consistencia);
	log_info(LOG_OUTPUT_SV,"Nombre tabla: %s Consistencia: %s Particiones: %d Tiempo de compactacion: %d", describe->nombre_tabla, consistencia, describe->particiones, describe->compactation_time);
	free(consistencia);
}

void log_instruccion_parseada(Instruccion * instruccion_parseada) {
	switch (instruccion_parseada->instruccion) {
		case RETORNO:;
		Retorno_Generico * retorno_generico = instruccion_parseada->instruccion_a_realizar;
			switch(retorno_generico->tipo_retorno){
			case VALOR:;
				Retorno_Value* retorno_value = retorno_generico->retorno;
				log_info(LOG_OUTPUT_SV,"Value: %s TS: %zu \n", retorno_value->value, retorno_value->timestamp);
				break;
			case SUCCESS:;
				log_info(LOG_OUTPUT_SV,"Operacion completada correctamente");
				break;
			case DATOS_DESCRIBE:
				list_iterate(((Describes*)((Retorno_Generico*)(instruccion_parseada->instruccion_a_realizar))->retorno)->lista_describes, (void*)log_describes);
				break;
			default:
				break;
			}
		break;
		case ERROR:;
			Error* error = instruccion_parseada->instruccion_a_realizar;
			switch (error->error) {
				case BAD_COMPACTATION:;
					log_error(LOG_ERROR_SV,"EL TIEMPO DE COMPACTACION DEBE SER UN NUMERO");
					break;

				case BAD_CONSISTENCY:;
					log_error(LOG_ERROR_SV,"TIPO DE CONSISTENCIA INCORRECTO [SC, SHC, EC]");
					break;

				case BAD_KEY:;
					log_error(LOG_ERROR_SV,"ERROR - NO EXISTE ESA KEY");
					break;

				case BAD_MEMORY:;
					log_error(LOG_ERROR_SV,"LA MEMORIA DEBE SER UN NUMERO");
					break;

				case BAD_OPERATION:;
					log_error(LOG_ERROR_SV,"OPERACION INVALIDA");
					break;

				case BAD_PARTITION:;
					log_error(LOG_ERROR_SV,"LA PARTICION DEBE SER UN NUMERO");
					break;

				case BAD_REQUEST:;
					log_error(LOG_ERROR_SV,"SINTAXIS INCORRECTA");
					break;

				case CONNECTION_ERROR:;
					log_error(LOG_ERROR_SV,"ERROR DE CONEXION");
					break;

				case JOURNAL_FAILURE:;
					log_error(LOG_ERROR_SV,"ERROR AL EJECUTAR JOURNAL");
					break;

				case MISSING_TABLE:;
					log_error(LOG_ERROR_SV,"LA TABLA NO EXISTE");
					break;

				case LARGE_VALUE:;
					log_error(LOG_ERROR_SV,"EL VALOR ES DEMASIADO LARGO");
				break;

				case TABLE_EXIST:;
					log_error(LOG_ERROR_SV,"LA TABLA YA EXISTE");
				break;

				case UNKNOWN:;
					log_error(LOG_ERROR_SV,"ERROR DESCONOCIDO");
				break;

				case MISSING_FILE:;
					log_error(LOG_ERROR_SV,"NO EXISTE EL ARCHIVO");
				break;

				case FILE_DELETE_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDO ELIMINAR EL ARCHIVO");
				break;

				case FILE_OPEN_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDO ABRIR EL ARCHIVO");
				break;

				case DIR_OPEN_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDO ABRIR EL DIRECTORIO");
				break;

				case DIR_DELETE_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDO ELIMINAR EL DIRECTORIO");
				break;

				case DIR_CREATE_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDO CREAR EL DIRECTORIO");
				break;

				case BLOCK_ASSIGN_ERROR:;
					log_error(LOG_ERROR_SV,"NO SE PUDIERON ASIGNAR LOS BLOQUES");
				break;
				case BLOCK_MAX_REACHED:;
					log_error(LOG_ERROR_SV,"NO SE PUDIERON ASIGNAR LOS BLOQUES - MAXIMO ALCANZADO");
				break;

				case NULL_REQUEST:;
					log_error(LOG_ERROR_SV,"REQUEST NO VALIDO");
				break;

				case DIV_BY_ZERO:;
					log_error(LOG_ERROR_SV,"SE INTENTO DIVIDIR POR 0");
					break;

				default:
					break;
			}
			break;

		default:
			break;
		}
}

void print_instruccion_parseada(Instruccion * instruccion_parseada) {
	switch (instruccion_parseada->instruccion) {
		case RETORNO:;
		Retorno_Generico * retorno_generico = instruccion_parseada->instruccion_a_realizar;
			switch(retorno_generico->tipo_retorno){
			case VALOR:;
				Retorno_Value* retorno_value = retorno_generico->retorno;
				log_info(LOG_OUTPUT,"Value: %s TS: %zu", retorno_value->value, retorno_value->timestamp);
				free(retorno_value->value);
				free(retorno_value);
				free(retorno_generico);
				free(instruccion_parseada);
				break;
			case SUCCESS:;
				log_info(LOG_OUTPUT,"Operacion completada correctamente.");
				free(retorno_generico);
				free(instruccion_parseada);
				break;
			case DATOS_DESCRIBE:
				log_info(LOG_OUTPUT,"Operacion completada correctamente.");
				list_iterate(((Describes*)((Retorno_Generico*)(instruccion_parseada->instruccion_a_realizar))->retorno)->lista_describes, (void*)show_describes);
				list_destroy_and_destroy_elements(((Describes*)((Retorno_Generico*)(instruccion_parseada->instruccion_a_realizar))->retorno)->lista_describes, (void*)eliminar_describe);
				free(((Describes*)((Retorno_Generico*)(instruccion_parseada->instruccion_a_realizar))->retorno));
				free((Retorno_Generico*)(instruccion_parseada->instruccion_a_realizar));
				free(instruccion_parseada);
				break;
			default:
				break;
			}
		break;
		case ERROR:;
			Error* error = instruccion_parseada->instruccion_a_realizar;
			switch (error->error) {
				case BAD_COMPACTATION:;
					log_error(LOG_ERROR,"EL TIEMPO DE COMPACTACION DEBE SER UN NUMERO");
					break;

				case BAD_CONSISTENCY:;
					log_error(LOG_ERROR,"TIPO DE CONSISTENCIA INCORRECTO [SC, SHC, EC]");
					break;

				case BAD_KEY:;
					log_error(LOG_ERROR,"ERROR - NO EXISTE ESA KEY");
					break;

				case BAD_MEMORY:;
					log_error(LOG_ERROR,"LA MEMORIA DEBE SER UN NUMERO");
					break;

				case BAD_OPERATION:;
					log_error(LOG_ERROR,"OPERACION INVALIDA");
					break;

				case BAD_PARTITION:;
					log_error(LOG_ERROR,"LA PARTICION DEBE SER UN NUMERO");
					break;

				case BAD_REQUEST:;
					log_error(LOG_ERROR,"SINTAXIS INCORRECTA");
					break;

				case CONNECTION_ERROR:;
					log_error(LOG_ERROR,"ERROR DE CONEXION");
					break;

				case JOURNAL_FAILURE:;
					log_error(LOG_ERROR,"ERROR AL EJECUTAR JOURNAL");
					break;

				case MISSING_TABLE:;
					log_error(LOG_ERROR,"LA TABLA NO EXISTE");
					break;

				case LARGE_VALUE:;
					log_error(LOG_ERROR,"EL VALOR ES DEMASIADO LARGO");
				break;

				case TABLE_EXIST:;
					log_error(LOG_ERROR,"LA TABLA YA EXISTE");
				break;

				case UNKNOWN:;
					log_error(LOG_ERROR,"ERROR DESCONOCIDO");
				break;

				case MISSING_FILE:;
					log_error(LOG_ERROR,"NO EXISTE EL ARCHIVO");
				break;

				case FILE_DELETE_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO ELIMINAR EL ARCHIVO");
				break;

				case FILE_OPEN_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO ABRIR EL ARCHIVO");
				break;

				case FILE_SYNC_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO SINCRONIZAR EL ARCHIVO");
				break;

				case DIR_OPEN_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO ABRIR EL DIRECTORIO");
				break;

				case DIR_DELETE_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO ELIMINAR EL DIRECTORIO");
				break;

				case DIR_CREATE_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDO CREAR EL DIRECTORIO");
				break;

				case BLOCK_ASSIGN_ERROR:;
					log_error(LOG_ERROR,"NO SE PUDIERON ASIGNAR LOS BLOQUES");
				break;
				case BLOCK_MAX_REACHED:;
					log_error(LOG_ERROR,"NO SE PUDIERON ASIGNAR LOS BLOQUES - MAXIMO ALCANZADO");
				break;

				case NULL_REQUEST:;
					log_error(LOG_ERROR,"REQUEST NO VALIDO");
				break;

				case DIV_BY_ZERO:;
					log_error(LOG_ERROR,"SE INTENTO DIVIDIR POR 0");
					break;

				default:
					break;
			}
			free(error);
			free(instruccion_parseada);
			break;

		default:
			break;
		}
}

bool es_select(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "SELECT"));
}

bool es_insert(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "INSERT"));
}

bool es_create(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "CREATE"));
}

bool es_describe(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "DESCRIBE"));
}

bool es_drop(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "DROP"));
}

bool es_add(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "ADD"));
}

bool es_run(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "RUN"));
}

bool es_metrics(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "METRICS"));
}

bool es_journal(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "JOURNAL"));
}

bool es_error(char** consulta_parseada) {
	return (string_equals_ignore_case(consulta_parseada[0], "ERROR"));
}

unsigned long int string_to_ulint(char* string) {
	long int dec = 0;
	int i, len;

	len = strlen(string);
	for (i = 0; i < len; i++) {
		dec = dec * 10 + (string[i] - '0');
	}

	return dec;
}

void leer_por_consola(void (*f)(char*)) {
	char* leido;

	leido = readline(">>");

	while (!string_equals_ignore_case(leido, "EXIT")) {
		if(!string_equals_ignore_case(leido, "\n")){
			add_history(leido);
			f(leido);
		}
		free(leido);
		leido = readline("\n>>");
	}

	free(leido);
	exit_gracefully(EXIT_SUCCESS);
}

t_timestamp get_timestamp() {
	struct timeval time;

	gettimeofday(&time,NULL);

	return (time.tv_sec * 1000 + time.tv_usec/1000);

}

void free_consulta_separada(int length,char** consulta_separada){
	for(int i=0;i<=length;i++){
		free(consulta_separada[i]);
	}
	free(consulta_separada);
}

void free_retorno(Instruccion * instruccion_parseada) {
	switch (instruccion_parseada->instruccion) {
		case RETORNO:;
		Retorno_Generico * retorno_generico = instruccion_parseada->instruccion_a_realizar;
			switch(retorno_generico->tipo_retorno){
			case VALOR:;
				Retorno_Value* retorno_value = retorno_generico->retorno;
				free(retorno_value->value);
				free(retorno_value);
				break;
			case RETORNO_GOSSIP:;
				list_destroy_and_destroy_elements(((Gossip*)retorno_generico->retorno)->lista_memorias, (void*)eliminar_memoria);
				free(retorno_generico->retorno);
				break;
			case DATOS_DESCRIBE:
				list_destroy_and_destroy_elements(((Describes*)retorno_generico->retorno)->lista_describes, (void*)eliminar_describe);
				free(((Describes*)retorno_generico->retorno));
				break;
			case TAMANIO_VALOR_MAXIMO:;
				free(retorno_generico->retorno);
				break;
			default:
				break;
			}
	default:
		break;
	}
	free(instruccion_parseada->instruccion_a_realizar);
	free(instruccion_parseada);
}

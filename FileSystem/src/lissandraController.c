#include "lissandra.h"

Instruccion* _insert(Insert* insert);
Instruccion* _select(Select* select);
Instruccion* _create(Create* create);
Instruccion* _describe(Describe* describe);
Instruccion* _drop(Drop* drop);

Instruccion* _insert(Insert* insert){
	usleep(global_conf.retardo*1000);
	if(!tableExists(insert->nombre_tabla)) {
		//log_error(LOG_ERROR,"INSERT: no existe la tabla '%s'",insert->nombre_tabla);
		free(insert->nombre_tabla);
		free(insert->value);
		free(insert);
		return respuesta_error(MISSING_TABLE);
	}
	if(strlen(insert->value)>global_conf.max_value_size) {
		//log_error(LOG_ERROR,"INSERT: value demasiado largo: '%d', maximo: '%d'",strlen(insert->value),global_conf.max_value_size);
		free(insert->nombre_tabla);
		free(insert->value);
		free(insert);
		return respuesta_error(LARGE_VALUE);
	}
	struct tableRegister reg = createTableRegister(insert->key,insert->value,insert->timestamp_insert);
	/* Espero posible Dump*/
	pthread_mutex_lock(&memtableMutex);
	/* Inserto valor en la memtable */
	insertInMemtable(createMemtableItem(insert->nombre_tabla,reg));
	/* desbloqueo mutex*/
	pthread_mutex_unlock(&memtableMutex);
	free(insert->nombre_tabla);
	free(insert->value);
	free(insert);
	return respuesta_success();
}
Instruccion* _select(Select* select){
	usleep(global_conf.retardo*1000);
	if(!tableExists(select->nombre_tabla)) {
		log_error(LOG_ERROR_SV,"SELECT: no existe la tabla '%s'",select->nombre_tabla);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(MISSING_TABLE);
	}
	struct tableMetadataItem* found = get_table_metadata(select->nombre_tabla);
	if(found == NULL) {
		log_error(LOG_ERROR_SV,"SELECT: No se encontro la metadata de la tabla '%s'",select->nombre_tabla);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(UNKNOWN);
	}

	t_list* listaRegistros = list_create();

	// Reviso Memtable
	findInMemtable(select->nombre_tabla,select->key,listaRegistros);

	if(found->metadata.numero_particiones == 0){
		clean_registers_list(listaRegistros);
		list_destroy(listaRegistros);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(DIV_BY_ZERO);
	}
	int particion = select->key%found->metadata.numero_particiones;
	char*particionFile = malloc(strlen(global_conf.directorio_tablas)+strlen(select->nombre_tabla)+digitos(particion)+6);
	sprintf(particionFile,"%s%s/%d.bin",global_conf.directorio_tablas,select->nombre_tabla,particion);

	// Espero posible compactacion
	pthread_rwlock_rdlock(&found->lock);

	// Reviso particion
	int retval = fs_read(particionFile,listaRegistros);
	if(retval != 0){
		clean_registers_list(listaRegistros);
		list_destroy(listaRegistros);
		pthread_rwlock_unlock(&found->lock);
		free(particionFile);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(retval);
	}
	free(particionFile);

	// Reviso archivos temporales
	retval = read_temp_files(select->nombre_tabla,listaRegistros);
	if(retval != 0){
		clean_registers_list(listaRegistros);
		list_destroy(listaRegistros);
		pthread_rwlock_unlock(&found->lock);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(retval);
	}

	//Levanto el bloqueo
	pthread_rwlock_unlock(&found->lock);
	
	// Busco el mayor
	struct tableRegister max;
	max.value=NULL;
	max.timestamp=0l;
	void lastest(struct tableRegister * reg){
		if(reg->key == select->key) {
			if(max.timestamp < reg->timestamp){ /// SERIO PROBLEMA CON EL MATCHING DE TIMESTAMPS
				if(max.value!=NULL){
					free(max.value);
				}
				max = createTableRegister(reg->key,reg->value,reg->timestamp);
			}
		}
	}
	list_iterate(listaRegistros,(void*)lastest);

	if(max.value==NULL){
		log_error(LOG_ERROR_SV,"SELECT: No existe la key %d en la tabla '%s'",select->key,select->nombre_tabla);
		clean_registers_list(listaRegistros);
		list_destroy(listaRegistros);
		free(max.value);
		free(select->nombre_tabla);
		free(select);
		return respuesta_error(BAD_KEY);
	}

	clean_registers_list(listaRegistros);
	list_destroy(listaRegistros);
	free(select->nombre_tabla);
	free(select);
	Instruccion * retornoInstruccion = armarRetornoValue(max.value,max.timestamp);
	free(max.value);
	return retornoInstruccion;
}
Instruccion* _create(Create* create){
	usleep(global_conf.retardo*1000);
	char*directorio=getTablePath(create->nombre_tabla);
	int retval = crearDirectorio(global_conf.directorio_tablas,create->nombre_tabla);
	if(retval != 0){
		free(create->nombre_tabla);
		free(create);
		free(directorio);
		return respuesta_error(retval);
	}
	retval = crearMetadataTableFile(directorio,setTableMetadata(create->consistencia,create->particiones,create->compactation_time));
	if(retval != 0) {
		free(directorio);
		free(create->nombre_tabla);
		free(create);
		return respuesta_error(retval);
	}
	retval = crearBinarios(directorio,create->particiones);
	if(retval != 0) {
		free(directorio);
		free(create->nombre_tabla);
		free(create);
		return respuesta_error(retval);
	}
	free(directorio);
	/* Agrego la nueva tabla a la metadata global */
	pthread_mutex_lock(&tableMetadataMutex);
	insertInTableMetadata(create->nombre_tabla,setTableMetadata(create->consistencia,create->particiones,create->compactation_time));
	pthread_mutex_unlock(&tableMetadataMutex);
	free(create->nombre_tabla);
	free(create);
	return respuesta_success();
}
Instruccion* _describe(Describe * describe){
	usleep(global_conf.retardo*1000);
	if(describe->nombre_tabla==NULL){
		t_list* lista_describes=list_create();
		loadDescribesTableMetadata(lista_describes);
		free(describe);
		return armarRetornoDescribe(lista_describes);
	}
	if(!tableExists(describe->nombre_tabla)) {
		log_error(LOG_ERROR_SV,"DESCRIBE: no existe la tabla '%s'",describe->nombre_tabla);
		free(describe->nombre_tabla);
		free(describe);
		return respuesta_error(MISSING_TABLE);
	}
	struct tableMetadataItem* found = get_table_metadata(describe->nombre_tabla);
	if(found==NULL){
		log_error(LOG_ERROR_SV,"DESCRIBE: No se encontro la metadata de la tabla %s",describe->nombre_tabla);
		free(describe->nombre_tabla);
		free(describe);
		return respuesta_error(UNKNOWN);
	}
	Retorno_Describe* actualDescribe = pack_describe(found->tableName,found->metadata.consistencia,found->metadata.numero_particiones,found->metadata.compaction_time);
	t_list* lista_describes=list_create();
	list_add(lista_describes,actualDescribe);
	free(describe->nombre_tabla);
	free(describe);
	return armarRetornoDescribe(lista_describes);
}
Instruccion* _drop(Drop* drop){
	usleep(global_conf.retardo*1000);
	if(!tableExists(drop->nombre_tabla)) {
		log_error(LOG_ERROR_SV,"DROP: no existe la tabla '%s'",drop->nombre_tabla);
		free(drop->nombre_tabla);
		free(drop);
		return respuesta_error(MISSING_TABLE);
	}
	pthread_mutex_lock(&tableMetadataMutex);
	deleteInTableMetadata(drop->nombre_tabla);
	pthread_mutex_unlock(&tableMetadataMutex);
	int retval = deleteTable(drop->nombre_tabla);
	if(retval != 0){
		free(drop->nombre_tabla);
		free(drop);
		return respuesta_error(retval);
	}
	free(drop->nombre_tabla);
	free(drop);
	return respuesta_success();
}
Instruccion* controller(Instruccion* instruccion){
	Instruccion* res;
	switch(instruccion->instruccion){
		case SELECT:
			res=_select((Select*)instruccion->instruccion_a_realizar);
		break;
		case INSERT:
			res=_insert((Insert*)instruccion->instruccion_a_realizar);
		break;
		case CREATE:
			res=_create((Create*)instruccion->instruccion_a_realizar);
		break;
		case DESCRIBE:
			res=_describe((Describe*)instruccion->instruccion_a_realizar);
		break;
		case DROP:
			res=_drop((Drop*)instruccion->instruccion_a_realizar);
		break;
		case MAX_VALUE:
			res=armarRetornoMaxValue();
		break;
		default:
			res=respuesta_error(BAD_REQUEST);
		break;
	}
	free(instruccion);
	return res;
}

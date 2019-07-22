#include "lissandra.h"

struct tableRegister createTableRegister(t_key key,char* value,t_timestamp timestamp) {
	struct tableRegister reg;
	reg.key=key;
	reg.value=malloc(global_conf.max_value_size);
	strcpy(reg.value,value);
	reg.timestamp = (timestamp==0)?getTimestamp():timestamp;
	return reg;
}

/*
	Global Conf
*/
void global_conf_load(t_config* conf){
	// Directorios
	char * p_m = config_get_string_value_check(conf,"PUNTO_MONTAJE");
	global_conf.directorio_tablas = malloc(strlen(p_m)+8+1);
	sprintf(global_conf.directorio_tablas,"%s/Tables/",p_m);
	global_conf.directorio_bloques = malloc(strlen(p_m)+9+1);
	sprintf(global_conf.directorio_bloques,"%s/Bloques/",p_m);
	global_conf.directorio_metadata = malloc(strlen(p_m)+10+1);
	sprintf(global_conf.directorio_metadata,"%s/Metadata/",p_m);
	// Puerto escucha
	char * p_e = config_get_string_value_check(conf,"PUERTO_ESCUCHA");
	global_conf.puerto=malloc(strlen(p_e)+1);
	strcpy(global_conf.puerto,p_e);
	// Retardo
	global_conf.retardo = config_get_int_value_check(conf,"RETARDO");
	// Value
	global_conf.max_value_size = config_get_int_value_check(conf,"MAX_VALUE_SIZE");
	// Dump
	global_conf.tiempo_dump = config_get_int_value_check(conf,"TIEMPO_DUMP");
}
void global_conf_update(t_config* conf){
	global_conf.retardo = config_get_int_value_check(conf,"RETARDO");
	global_conf.tiempo_dump = config_get_int_value_check(conf,"TIEMPO_DUMP");
}
void global_conf_destroy(void){
	free(global_conf.directorio_tablas);
	free(global_conf.directorio_bloques);
	free(global_conf.directorio_metadata);
	free(global_conf.puerto);
}

t_timestamp getTimestamp(void) {
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000+t.tv_usec/1000;
}
int tableExists(char * table){
	char* path = getTablePath(table);
	int retval = existeDirectorio(path);
	free(path);
	return retval;
}
int getNumLastFile(char* prefix,char* extension,char* path) {
	int retnum = 0;
	struct dirent *dir;
	DIR *d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG){
				//Verifico si empieza con el prefijo y termina con la extension correspondiente
				if(strstr(dir->d_name,prefix)==dir->d_name && strstr(dir->d_name,extension) == dir->d_name+(strlen(dir->d_name)-strlen(extension))) {				
					int currnum;
					//Seteo como fin de string justo cuando comienza la extension
					*(dir->d_name+(strlen(dir->d_name)-strlen(extension)-1))='\0';
					char*strnum=malloc(strlen(dir->d_name+strlen(prefix))+1);
					//Copio el valor numerico del archivo
					strcpy(strnum,dir->d_name+strlen(prefix));
					sscanf(strnum,"%d",&currnum);
					free(strnum);
					retnum = (currnum>retnum)?currnum:retnum;
				}
			}
		}
		closedir(d);
	}
	return retnum;
}

struct tableMetadataItem* get_table_metadata(char* tabla){
	bool criterioTablename(struct tableMetadataItem* t){
		if(t != NULL && t->tableName != NULL){
			return !strcmp(tabla,t->tableName);
		}else{
			return false;
		}
	}
	pthread_mutex_lock(&tableMetadataMutex);
	struct tableMetadataItem* found = list_find(global_table_metadata,(void*)criterioTablename);
	pthread_mutex_unlock(&tableMetadataMutex);
	return found;
}
int deleteTable(char* tabla){
	char*path = getTablePath(tabla);
	struct dirent *dir;
	DIR *d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG){
				char*filename=malloc(strlen(path)+strlen(dir->d_name)+2);
				sprintf(filename,"%s/%s",path,dir->d_name);
				if(strcmp(dir->d_name,"Metadata.txt")==0){
					if(remove(filename)){
						log_error(LOG_ERROR,"Error al eliminar el archivo '%s', %s",filename,strerror(errno));
						closedir(d);
						free(path);
						free(filename);
						return FILE_DELETE_ERROR;
					}
				} else {
					int retval = fs_delete(filename);
					if(retval != 0){
						closedir(d);
						free(path);
						free(filename);
						return retval;
					}
				}
				free(filename);
			}
		}
		closedir(d);
	} else {
		log_error(LOG_ERROR,"Error al abrir el directorio %s, %s",path, strerror(errno));
		free(path);
		return DIR_OPEN_ERROR;
	}
	if(remove(path)){
		log_error(LOG_ERROR,"Error al eliminar el directorio '%s', %s",path,strerror(errno));
		free(path);
		return DIR_DELETE_ERROR;
	}
	free(path);
	return 0;
}
void clean_registers_list(t_list*registers){
	void cleanValue(struct tableRegister* reg){
		free(reg->value);
		free(reg);
	}
	list_iterate(registers,(void*)cleanValue);
}
int read_temp_files(char* tabla,t_list* listaRegistros){
	char*path = getTablePath(tabla);
	struct dirent *dir;
	DIR *d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG){
				// Verifico que termine con .tmp o con .tmpc
				if(strstr(dir->d_name,".tmp") == dir->d_name+(strlen(dir->d_name)-strlen(".tmp")) || strstr(dir->d_name,".tmpc") == dir->d_name+(strlen(dir->d_name)-strlen(".tmpc"))){
					char*filename=malloc(strlen(path)+strlen(dir->d_name)+2);
					sprintf(filename,"%s/%s",path,dir->d_name);
					int retval = fs_read(filename,listaRegistros);
					if(retval != 0){
						closedir(d);
						free(path);
						free(filename);
						return retval;
					}
					free(filename);
				}
			}
		}
		closedir(d);
	} else {
		log_error(LOG_ERROR,"Error al abrir el directorio %s, %s",path,strerror(errno));
		free(path);
		return DIR_OPEN_ERROR;
	}
	free(path);
	return 0;
}
char* getTablePath(char*tabla){
	char*path = malloc(strlen(global_conf.directorio_tablas)+strlen(tabla)+1);
	sprintf(path,"%s%s",global_conf.directorio_tablas,tabla);
	return path;
}
int digitos(int num){
	int l=!num;
	while(num){l++; num/=10;}
	return l;
}
int digitos_long(long num){
	long l=!num;
	while(num){l++; num/=10;}
	return (int)l;
}
/* Describe */
Retorno_Describe* pack_describe(char *nombre_tabla,Consistencias consistencia,uint8_t particiones,t_timestamp compactation_time){
	Retorno_Describe* respuesta = malloc(sizeof(Retorno_Describe));
	respuesta->nombre_tabla=malloc(strlen(nombre_tabla)+1);
	strcpy(respuesta->nombre_tabla,nombre_tabla);
	respuesta->consistencia=consistencia;
	respuesta->particiones=particiones;
	respuesta->compactation_time=compactation_time;
	return respuesta;
}
void showDescribeList(Retorno_Describe* describe){
	char* consistencia=consistencia2string(describe->consistencia);
	log_info(LOG_OUTPUT,"Nombre tabla: %s Consistencia: %s Particiones: %d Tiempo de compactacion: %d\n",describe->nombre_tabla,consistencia,describe->particiones,describe->compactation_time);
	free(consistencia);
}

/* Armar Instruccion */
Instruccion* armarRetornoValue(char *value,t_timestamp timestamp){
	Instruccion* instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion=RETORNO;
	Retorno_Generico * retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno=VALOR;
	Retorno_Value * retval = malloc(sizeof(Retorno_Value));
	retval->value = malloc(strlen(value)+1);
	strcpy(retval->value,value);
	retval->timestamp=timestamp;
	retorno->retorno=retval;
	instruccion->instruccion_a_realizar=retorno;
	return instruccion;
}
Instruccion* armarRetornoDescribe(t_list* lista_describes){
	Instruccion* instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion=RETORNO;
	Retorno_Generico* retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno=DATOS_DESCRIBE;
	Describes* describes = malloc(sizeof(Describes));
	describes->lista_describes = lista_describes;
	retorno->retorno=describes;
	instruccion->instruccion_a_realizar=retorno;
	return instruccion;
}
Instruccion* armarRetornoMaxValue(void){
	Instruccion* instruccion = malloc(sizeof(Instruccion));
	instruccion->instruccion=RETORNO;
	Retorno_Generico* retorno = malloc(sizeof(Retorno_Generico));
	retorno->tipo_retorno=TAMANIO_VALOR_MAXIMO;
	Retorno_Max_Value* max_value = malloc(sizeof(Retorno_Max_Value));
	max_value->value_size = global_conf.max_value_size;
	retorno->retorno=max_value;
	instruccion->instruccion_a_realizar=retorno;
	return instruccion;
}
void deleteDescribeList(Retorno_Describe* describe){
	free(describe->nombre_tabla);
	free(describe);
}

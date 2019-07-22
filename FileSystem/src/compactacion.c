#include "lissandra.h"

int compac_search_tmp_files(t_list* tmpc_files, char* tableName){
	char*path=getTablePath(tableName);
	struct dirent *dir;
	DIR *d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG){
				//Verifico si empieza con el prefijo y termina con la extension correspondiente
				if(strstr(dir->d_name,"dump_")==dir->d_name && strstr(dir->d_name,"tmp") == dir->d_name+(strlen(dir->d_name)-strlen("tmp"))) {
					char*newfilename = malloc(strlen(path)+1+strlen(dir->d_name)+2); 
					sprintf(newfilename,"%s/%sc",path,dir->d_name);
					char*oldfilename = malloc(strlen(path)+1+strlen(dir->d_name)+1);
					sprintf(oldfilename,"%s/%s",path,dir->d_name);
					if (rename(oldfilename,newfilename)!=0){
						log_error(LOG_ERROR,"Error al renombrar el archivo %s a %s, %s",dir->d_name,newfilename,strerror(errno));
						free(newfilename);
						free(oldfilename);
						free(path);
						return 1;
					}
					free(oldfilename);
					list_add(tmpc_files,newfilename);
				}
			}
		}
		closedir(d);
	} else{
		log_error(LOG_ERROR,"Error al abrir el directorio %s, %s",path,strerror(errno));
		free(path);
		return 1;
	}
	free(path);
	return 0;
}
int compac_get_tmp_registers(t_list*registrosTemporales,t_list*tmpc_files){
	int ret=0;
	void getRegisters(char* file){
		ret = fs_read(file,registrosTemporales);
	}
	list_iterate(tmpc_files,(void*)getRegisters);
	return ret;
}
int compac_get_partition_registers(t_list*registrosParticiones,char*tableName,int numero_particiones){
	char*basePath=getTablePath(tableName);
	for(int i=0;i<numero_particiones;i++){
		char*filename = malloc(strlen(basePath)+1+digitos(i)+5);
		sprintf(filename,"%s/%d.bin",basePath,i);
		t_list* particion=list_create();
		if(fs_read(filename,particion)!=0){
			list_destroy(particion);
			free(filename);
			free(basePath);
			return 1;
		}
		list_add_in_index(registrosParticiones,i,(void*)particion);
		free(filename);
	}
	free(basePath);
	return 0;
}
void compac_match_registers(t_list*registrosParticiones,t_list*registrosTemporales){
	int cantParticiones = list_size(registrosParticiones);
	void iterateRegistersTmp(struct tableRegister* tempReg){
		if(cantParticiones == 0){
			log_error(LOG_ERROR,"SE INTENTO DIVIDIR POR 0");
			return;
		}
		int partNum = tempReg->key%cantParticiones;
		t_list* partRegisters = (t_list*)list_get(registrosParticiones,partNum);
		if(partRegisters==NULL){
			return;
		}

		// Defino la funcion para buscar la key temporal dentro de los registros de la particion
		bool sameKey(struct tableRegister* partReg){
			return partReg->key==tempReg->key;
		}

		struct tableRegister* found = list_find(partRegisters,(void*)sameKey);
		if(found == NULL){ // Agrego el nuevo registro a la particion
			struct tableRegister* newReg = malloc(sizeof(struct tableRegister));
			*newReg=createTableRegister(tempReg->key,tempReg->value,tempReg->timestamp);
			list_add(partRegisters,(void*)newReg);
		} else { // me fijo cual es mas reciente (timestamp) y me quedo con ese
			if(found->timestamp<tempReg->timestamp) {
				struct tableRegister* removed = list_remove_by_condition(partRegisters,(void*)sameKey);
				free(removed->value);
				free(removed);
				struct tableRegister* newReg = malloc(sizeof(struct tableRegister));
				*newReg=createTableRegister(tempReg->key,tempReg->value,tempReg->timestamp);
				list_add(partRegisters,(void*)newReg);
			}
		}
	}
	list_iterate(registrosTemporales,(void*)iterateRegistersTmp);
}
int compac_delete_tmpc_files(t_list*tmpc_files){
	int retValue=0;
	void deleteFile(char * filename){
		if(fs_delete(filename)!=0){
			retValue=1;
		}
	}
	list_iterate(tmpc_files,(void*)deleteFile);
	return retValue;
}
int compac_save_partition_registers(t_list*registrosParticiones,char* tablename){
	// Borro los .bin anteriores
	for(int i=0;i<list_size(registrosParticiones);i++){
		char*filename=malloc(strlen(global_conf.directorio_tablas)+strlen(tablename)+1+digitos(i)+5);
		sprintf(filename,"%s%s/%d.bin",global_conf.directorio_tablas,tablename,i);
		if(fs_delete(filename)!=0){
			free(filename);
			return 1;
		}
		free(filename);
	}
	// Escribo los datos en los nuevos .bin
	for(int i=0;i<list_size(registrosParticiones);i++){
		char*filename=malloc(strlen(global_conf.directorio_tablas)+strlen(tablename)+1+digitos(i)+5);
		sprintf(filename,"%s%s/%d.bin",global_conf.directorio_tablas,tablename,i);
		t_list* particion = list_get(registrosParticiones,i);
		if(fs_write(filename,particion)!=0){
			free(filename);
			return 1;
		}
		free(filename);
	}
	return 0;
}
void compac_clean_partition_registers(t_list*registrosParticiones){
	void limpiar(t_list * particion){
		void cleanValue(struct tableRegister* reg){
			free(reg->value);
			free(reg);
		}
		list_iterate(particion,(void*)cleanValue);
		list_destroy(particion);
	}
	list_iterate(registrosParticiones,(void*)limpiar);
}
void compac_clean_temp_registers(t_list*registrosTemporales){
	void cleanValue(struct tableRegister* reg){
		free(reg->value);
		free(reg);
	}
	list_iterate(registrosTemporales,(void*)cleanValue);
}
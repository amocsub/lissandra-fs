#include "lissandra.h"

struct PartitionData{
	int size;
	t_list* blocks;
};

char * makeMetadataTableString(struct TableMetadata tableMeta);
struct PartitionData getPartitionData(char* nameTable, int partition);
int setPartitionData(struct PartitionData partitionD);

int crearDirectorio(char* source,char* name){
	char*PATH=malloc(strlen(source)+strlen(name)+2);
	sprintf(PATH,"%s/%s",source,name);

	if(!existeDirectorio(PATH)){
		if(mkdir(PATH,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)==0){ //Directorio creado con 755
			free(PATH);
			return 0;
		} else {
			log_error(LOG_ERROR_SV,"Error al crear el directorio '%s', %s",PATH,strerror(errno));
			free(PATH);
			return DIR_CREATE_ERROR;
		}
	}
	log_error(LOG_ERROR_SV,"Tabla '%s' existente",name);
	free(PATH);
    return TABLE_EXIST;
}

int mkdirRecursivo(char*pathSrc) { //Creados con 755
	//Copia del pathsrc
	char* path = malloc(strlen(pathSrc)+1);
	strcpy(path,pathSrc);
	//Acumulador de directorios creados
	char* create = malloc(strlen(pathSrc)+2);
	strcpy(create,"/");
	//Spliteo el path
	char* find = strtok(path,"/");
	while(find != NULL) {
		strcat(create,find);
		if(!existeDirectorio(create)) {
			if(mkdir(create,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0){
				log_error(LOG_ERROR_SV,"Error al crear el directorio '%s', %s",create,strerror(errno));
				free(create);
				free(path);
				return DIR_CREATE_ERROR;
			}
		}
		strcat(create,"/");
		find = strtok(NULL,"/");
	}
	free(create);
	free(path);
	return 0;
}
char* makeMetadataTableString(struct TableMetadata tableMeta){
	char*consistencia=consistencia2string(tableMeta.consistencia);
	char*metadataStr=malloc(12+strlen(consistencia)+12+digitos(tableMeta.numero_particiones)+17+digitos(tableMeta.compaction_time)+1);
	sprintf(metadataStr,"CONSISTENCY=%s\nPARTITIONS=%d\nCOMPACTION_TIME=%d",consistencia,tableMeta.numero_particiones,tableMeta.compaction_time);
	free(consistencia);
	return metadataStr;
}
int crearMetadataTableFile(char*directorio,struct TableMetadata tableMeta){
	char*PATH = malloc(strlen(directorio)+13+1);
	sprintf(PATH,"%s/Metadata.txt",directorio);
	FILE *pf = txt_open_for_append(PATH);
	free(PATH);
	if (pf == NULL){
		log_error(LOG_ERROR_SV,"Error al abrir el archivo '%s', %s",PATH,strerror(errno));
		return FILE_OPEN_ERROR;
	}
	char * data = makeMetadataTableString(tableMeta);
	txt_write_in_file(pf, data);
	txt_close_file(pf);
	free(data);
	return 0;
}
int crearBinarios(char*directorio,int numero_particiones){
	int n=0;
	while(n<numero_particiones){
		char*filename=malloc(strlen(directorio)+1+digitos(numero_particiones)+5);
		sprintf(filename,"%s/%d.bin",directorio,n);
		if(fs_create(filename)!=0){
			log_error(LOG_ERROR_SV,"Error al crear el archivo '%s', %s",filename,strerror(errno));
			free(filename);
			return FILE_OPEN_ERROR;
		}
		free(filename);
		n++;
	}
	return 0;
}
struct TableMetadata setTableMetadata(Consistencias consistencia, int numero_particiones,double compaction_time){
	struct TableMetadata mData;
	mData.consistencia = consistencia;
	mData.numero_particiones = numero_particiones;
	mData.compaction_time = compaction_time;
	return mData;
}

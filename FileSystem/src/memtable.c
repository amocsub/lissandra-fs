#include "lissandra.h"


void* TH_asesino(void*p);

struct memtableItem* createMemtableItem(char* tableName, struct tableRegister reg) {
	struct memtableItem * item = malloc(sizeof(struct memtableItem));
	item->reg = reg;
	item->tableName = malloc(strlen(tableName)+1);
	strcpy(item->tableName,tableName);
	return item;
}
void initMemtable(void){
	global_memtable = list_create();
}
void insertInMemtable(struct memtableItem* item) {
	list_add(global_memtable, (void *)item);
}
void cleanMemtable(void){
	void freeValues(void* elem) {
		struct memtableItem* item = (struct memtableItem*)elem;
		free(item->reg.value);
		free(item->tableName);
		free(item);
	}
	list_iterate(global_memtable, &freeValues);
	list_clean(global_memtable);
}
void destroyMemtable(void){
	cleanMemtable();
	list_destroy(global_memtable);
}
void findInMemtable(char* tablename, uint16_t key, t_list* registers){
	void matchReg(struct memtableItem * item){
		if (strcmp(item->tableName,tablename)==0 && item->reg.key == key) {
			struct tableRegister* newreg = malloc(sizeof(struct tableRegister));
			*newreg = createTableRegister(item->reg.key,item->reg.value,item->reg.timestamp);
			list_add(registers,(void*)newreg);
		}
	}
	pthread_mutex_lock(&memtableMutex);
	list_iterate(global_memtable,(void*)matchReg);
	pthread_mutex_unlock(&memtableMutex);
}

/* Global Table metadata*/
void initTableMetadata(void){
	global_table_metadata = list_create();
	loadCurrentTableMetadata();
}
void insertInTableMetadata(char*tableName,struct TableMetadata tMetadata) {
	bool criterio(struct tableMetadataItem* t){
		return !strcmp(t->tableName,tableName);
	}
	struct tableMetadataItem* old = list_find(global_table_metadata,(void*)criterio);
	if(old != NULL){
		list_remove_by_condition(global_table_metadata,(void*)criterio);
	}
	struct tableMetadataItem* item = malloc(sizeof(struct tableMetadataItem));
	item->metadata=tMetadata;
	item->tableName = malloc(strlen(tableName)+1);
	strcpy(item->tableName,tableName);
	pthread_rwlock_init(&item->lock,NULL);
	item->endFlag=0;
	pthread_create(&item->thread,NULL,TH_compactacion,(void*)item);
	list_add(global_table_metadata, (void *)item);
}
void deleteInTableMetadata(char*tableName) {
	bool condition(struct tableMetadataItem* item){
		return !strcmp(tableName,item->tableName);
	}
	struct tableMetadataItem* found = list_find(global_table_metadata,(void*)condition);
	if (found != NULL) {
		/* Hilo asesino */
		pthread_t hilo_asesino;
		pthread_create(&hilo_asesino,NULL,TH_asesino,(void*)found);
		pthread_detach(hilo_asesino);
		list_remove_by_condition(global_table_metadata,(void*)condition);
	}
}
void destroyTableMetadata(void){
	void destroyMutex(struct tableMetadataItem * item){
		pthread_rwlock_destroy(&item->lock);
	}
	list_iterate(global_table_metadata,(void*)destroyMutex);
	list_destroy(global_table_metadata);
}
void loadCurrentTableMetadata(void){
	struct dirent *dir;
	DIR *d = opendir(global_conf.directorio_tablas);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){
				char*filename=malloc(strlen(global_conf.directorio_tablas)+strlen(dir->d_name)+1+12+1);
				sprintf(filename,"%s%s/Metadata.txt",global_conf.directorio_tablas,dir->d_name);
				t_config* conf = config_create(filename);
				if(conf == NULL){
					free(filename);
					continue;
				}
				struct tableMetadataItem *item = malloc(sizeof(struct tableMetadataItem));
				pthread_rwlock_init(&item->lock,NULL);
				item->endFlag=0;
				item->tableName = malloc(strlen(dir->d_name)+1);
				strcpy(item->tableName,dir->d_name);
				item->metadata.consistencia=string2consistencia(config_get_string_value(conf,"CONSISTENCY"));
				item->metadata.numero_particiones=config_get_int_value(conf,"PARTITIONS");
				item->metadata.compaction_time=config_get_int_value(conf,"COMPACTION_TIME");
				config_destroy(conf);
				free(filename);
				pthread_mutex_lock(&tableMetadataMutex);
				list_add(global_table_metadata,(void *)item);
				pthread_mutex_unlock(&tableMetadataMutex);
				pthread_create(&item->thread,NULL,TH_compactacion,(void*)item);
			}
		}
		closedir(d);
	}
}
void loadDescribesTableMetadata(t_list*lista_describes){
	void show(struct tableMetadataItem * item){
		list_add(lista_describes,pack_describe(item->tableName,item->metadata.consistencia,item->metadata.numero_particiones,item->metadata.compaction_time));
	}
	pthread_mutex_lock(&tableMetadataMutex);
	list_iterate(global_table_metadata,(void*)show);
	pthread_mutex_unlock(&tableMetadataMutex);
}
void* TH_asesino(void*p){
	struct tableMetadataItem* item = (struct tableMetadataItem*)p;
	item->endFlag=1;
	pthread_join(item->thread,NULL);
	pthread_rwlock_destroy(&item->lock);
	free(item->tableName);
	free(item);
	return (void*)0;
}
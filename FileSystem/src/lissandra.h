#ifndef LISSANDRA_H
#define LISSANDRA_H
/*
	UtilGueguencha
*/
#include "../../utilguenguencha/src/comunicacion.h"
#include "../../utilguenguencha/src/parser.h"
#include "../../utilguenguencha/src/utils.h"
/*
	Commons
*/
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
/*
	Unix
*/
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>



/*
	Global Config
*/
struct lfsConfig{
	char*puerto;
	int retardo;
	int max_value_size;
	int tiempo_dump;
	char*directorio_tablas;
	char*directorio_bloques;
	char*directorio_metadata;
};
struct lfsConfig global_conf;
void global_conf_load(t_config* conf);
void global_conf_update(t_config* conf);
void global_conf_destroy(void);


/*
	Table Metadata
*/
struct TableMetadata{
	Consistencias consistencia;
	int numero_particiones;
	int compaction_time;
};

struct tableMetadataItem {
	struct TableMetadata metadata;
	char* tableName;
	pthread_rwlock_t lock;
	pthread_t thread;
	int endFlag;
};

struct FileSystemMetadata{
	int block_size;
	int blocks;
	int magic_number;
};

/* Tables */
struct tableRegister{
	t_key key;
	char* value;
	t_timestamp timestamp;
};
struct memtableItem{
	struct tableRegister reg;
	char* tableName;
};
/* Dump Memtable Struct*/
struct dumpTableList{
	char* tableName;
	t_list* registros;
};

/* Global Variables*/
t_list* global_memtable;
t_list* global_table_metadata;
pthread_mutex_t memtableMutex;
pthread_mutex_t tableMetadataMutex;

/* Thread Compactacion */
void *TH_compactacion(void* p);

/* API */
Instruccion* controller(Instruccion* instruccion);

/* Memtable */
void initMemtable(void);
void cleanMemtable(void);
void destroyMemtable(void);
void insertInMemtable(struct memtableItem* item);
struct memtableItem* createMemtableItem(char* tableName, struct tableRegister reg);
void findInMemtable(char* tablename, uint16_t key, t_list* registers);

/* Global Table metadata*/
void initTableMetadata(void);
void insertInTableMetadata(char*tableName,struct TableMetadata tMetadata);
void deleteInTableMetadata(char*tableName);
void destroyTableMetadata(void);
void loadCurrentTableMetadata(void);
void loadDescribesTableMetadata(t_list*lista_describes);

/* Registros */
t_timestamp getTimestamp(void);
struct tableRegister createTableRegister(t_key key,char* value,t_timestamp timestamp);

/* Utils */
int tableExists(char * table);
int getNumLastFile(char* prefix,char* extension,char* path);
struct tableMetadataItem* get_table_metadata(char* tabla);
int deleteTable(char* tabla);
void clean_registers_list(t_list*registers);
int read_temp_files(char* tabla,t_list* listaRegistros);
char* getTablePath(char*tabla);
int digitos(int num);
int digitos_long(long num);
void deleteDescribeList(Retorno_Describe* describe);

/* Armar Instruccion */
Instruccion* armarRetornoValue(char *value,t_timestamp timestamp);
Instruccion *armarRetornoDescribe(t_list* lista_describes);
Instruccion* armarRetornoMaxValue(void);

/* Describe */
Retorno_Describe* pack_describe(char *nombre_tabla,Consistencias consistencia,uint8_t particiones,t_timestamp compactation_time);
void showDescribeList(Retorno_Describe* describe);

/* Fmanager */
int existeDirectorio(char path[]);
int crearDirectorio(char*source, char*name);
int crearMetadataTableFile(char*directorio,struct TableMetadata tableMeta);
int crearMetadataFile(char*directorio,struct FileSystemMetadata systemMeta);
int crearBinarios(char*directorio,int numero_particiones);
int mkdirRecursivo(char*path);
struct TableMetadata setTableMetadata(Consistencias consistencia, int numero_particiones,double compaction_time);

/* Filesystem */
int fs_init(void);
void fs_destroy(void);
/*
	fs_read() : implementacion FS de lectura de archivo
		char* filename : nombre de archivo a abrir
		t_list* registros : lista donde va a escribir los registros leidos, items en formato "struct tableRegister"
	return value: 0 en caso de exito, de lo contrario 1
*/
int fs_read(char* filename, t_list* registros);
/*
	fs_write() : implementacion FS de lectura de archivo
		char* filename : nombre de archivo a escribir
		t_list* registros : lista con items en formato "struct tableRegister" a escribir en el archivo
	return value: 0 en caso de exito, de lo contrario 1
*/
int fs_write(char* filename, t_list* registros);
/*
	fs_create() : implementacion FS de creacion de archivo
		char* filename : nombre de archivo a crear
	return value: 0 en caso de exito, de lo contrario 1
*/
int fs_create(char* filename);
/*
	fs_delete() : implementacion FS de borrado de archivo
		char* filename : nombre de archivo a borrar
	return value: 0 en caso de exito, de lo contrario 1
*/
int fs_delete(char* filename);

/* Compactacion */
int compac_search_tmp_files(t_list* tmpc_files, char* tableName);
int compac_get_tmp_registers(t_list*registrosTemporales,t_list*tmpc_files);
int compac_get_partition_registers(t_list*registrosParticiones,char*tableName,int numero_particiones);
void compac_match_registers(t_list*registrosParticiones,t_list*registrosTemporales);
int compac_save_partition_registers(t_list*registrosParticiones,char* tablename);
int compac_delete_tmpc_files(t_list*tmpc_files);
void compac_clean_partition_registers(t_list*registrosParticiones);

#endif /*LISSANDRA_H*/

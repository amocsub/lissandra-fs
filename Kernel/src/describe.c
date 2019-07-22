#include "kernel.h"
void describe_agregar_tablas_nuevas(t_list * describes);
void describe_eliminar_tablas_viejas(t_list * describes);
void describe_eliminar_tabla(char*nombre_tabla,Consistencias cs);

void realizarDescribeGeneral(void){
	Instruccion * instruccionDescribe = malloc(sizeof(Instruccion));
	Describe * describe = malloc(sizeof(Describe));
	describe->nombre_tabla = NULL;

	instruccionDescribe->instruccion = DESCRIBE;
	instruccionDescribe->instruccion_a_realizar = (void *) describe;
	Instruccion * describeResponse = enviar_instruccion(IP_MEMORIA_PPAL,PUERTO_MEMORIA_PPAL,instruccionDescribe, KERNEL, T_INSTRUCCION);

	if(describeResponse->instruccion == RETORNO){
		switch(((Retorno_Generico*)describeResponse->instruccion_a_realizar)->tipo_retorno){
			case DATOS_DESCRIBE:;
				t_list * describes = ((Describes*)((Retorno_Generico*)describeResponse->instruccion_a_realizar)->retorno)->lista_describes;
				describe_agregar_tablas_nuevas(describes);
				describe_eliminar_tablas_viejas(describes);
			break;
			default:
				log_error(LOG_ERROR_SV, "Hilo Describe: Respuesta retorno inesperada\n");
			break;
		}
	}
	free_retorno(describeResponse);
}

void describe_agregar_tablas_nuevas(t_list * describes){
	// Defino funcion para agregar tablas nuevas
	void agregar_tablas_nuevas(Retorno_Describe * newtabla){
		// Defino criterio igual nombre, igual consistencia
		bool criterioBusqueda(Table_Metadata* tabla) {
			return !strcmp(tabla->tablename,newtabla->nombre_tabla) && tabla->consistencia==newtabla->consistencia;
		}
		Table_Metadata* found = list_find(lista_de_tablas,(void*)criterioBusqueda);
		if(found == NULL){
			Table_Metadata * agregartabla = malloc(sizeof(Table_Metadata));
			agregartabla->tablename = malloc(strlen(newtabla->nombre_tabla)+1);
			strcpy(agregartabla->tablename,newtabla->nombre_tabla);
			agregartabla->consistencia = newtabla->consistencia;
			pthread_mutex_lock(&lista_de_tablas_mx);
			list_add(lista_de_tablas,(void*)agregartabla);
			pthread_mutex_unlock(&lista_de_tablas_mx);
		}
	}
	// Agrego tablas nuevas
	list_iterate(describes,(void*)agregar_tablas_nuevas);
}

void describe_eliminar_tablas_viejas(t_list * describes){
	// Defino funcion para eliminar viejas tablas
	void eliminar_tablas_viejas(Table_Metadata* tabla){
		// Defino criterio igual nombre, igual consistencia
		bool criterioBusqueda(Retorno_Describe* newtabla) {
			return !strcmp(tabla->tablename,newtabla->nombre_tabla) && tabla->consistencia==newtabla->consistencia;
		}
		Retorno_Describe* found = list_find(describes,(void*)criterioBusqueda);
		if(found == NULL){ // Si no esta, la elimino
			describe_eliminar_tabla(tabla->tablename,tabla->consistencia);
		}
	}
	// Elimino viejas tablas
	list_iterate(lista_de_tablas,(void*)eliminar_tablas_viejas);
}
void describe_eliminar_tabla(char*nombre_tabla,Consistencias cs){
	bool removecondition(Table_Metadata* eliminar){
		if(!strcmp(nombre_tabla,eliminar->tablename) && eliminar->consistencia == cs) {
			free(eliminar->tablename);
			free(eliminar);
			return true;
		}
		return false;
	}
	pthread_mutex_lock(&lista_de_tablas_mx);
	list_remove_by_condition(lista_de_tablas,(void*)removecondition);
	pthread_mutex_unlock(&lista_de_tablas_mx);
}

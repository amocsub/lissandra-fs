#include "utils.h"

void configure_logger() {
	signal(SIGPIPE, handler);
	LOG_INFO = log_create("log_info.log","tp-lissandra", 0, LOG_LEVEL_INFO);
	LOG_ERROR = log_create("log_error.log","tp-lissandra", 1, LOG_LEVEL_ERROR);
	LOG_OUTPUT = log_create("log_output.log", "tp-lissandra", 0, LOG_LEVEL_INFO);
	LOG_ERROR_SV = log_create("log_error_sv.log","tp-lissandra", 0, LOG_LEVEL_ERROR);
	LOG_OUTPUT_SV = log_create("log_output_sv.log", "tp-lissandra", 0, LOG_LEVEL_INFO);
	LOG_DEBUG = log_create("log_debug.log","tp-lissandra", 1, LOG_LEVEL_DEBUG);
}

void exit_gracefully(int exit_code){
	if(exit_code == EXIT_FAILURE){
		log_error(LOG_ERROR,strerror(errno));
	}
	else{
		log_info(LOG_INFO,"Proceso termino correctamente");
	}
	dictionary_destroy_and_destroy_elements(fd_disponibles, (void*)free);
	log_destroy(LOG_INFO);
	log_destroy(LOG_ERROR);
	log_destroy(LOG_ERROR_SV);
	log_destroy(LOG_DEBUG);
	log_destroy(LOG_OUTPUT);
	log_destroy(LOG_OUTPUT_SV);
	exit(exit_code);
}

void handler(int s) {
	//No se que pasa aca
}

char *consistencia2string(Consistencias consistencia){
	char* str=malloc(4);
	switch(consistencia){
		case EC:
			strcpy(str,"EC");
		break;
		case SC:
			strcpy(str,"SC");
		break;
		case SHC:
			strcpy(str,"SHC");
		break;
		default:
			strcpy(str,"S/D");
		break;
	}
	return str;
}

int string2consistencia(char* consistencia){
	if(strcmp(consistencia,"EC")==0){
		return EC;
	}
	if(strcmp(consistencia,"SC")==0){
		return SC;
	}
	if(strcmp(consistencia,"SHC")==0){
		return SHC;
	}
	return DISP;
}

int monitorNode(char * node,int mode,int(*callback)(void)){
	char buffer[EVENT_BUF_LEN];
	int infd = inotify_init();
	if (infd < 0) {
		log_error(LOG_ERROR,"Problemas al crear el inotify para %s, %s",node,strerror(errno));
		return 1;
	}
	int wfd = inotify_add_watch(infd,node,mode);
	while(true){
		int length = read(infd,buffer,EVENT_BUF_LEN);
		if (length < 0) {
			log_error(LOG_ERROR,"Problemas al leer el inotify de %s, %s",node,strerror(errno));
			inotify_rm_watch(infd,wfd);
			close(infd);
			return 1;
		}
		if((*callback)()!=0){
			inotify_rm_watch(infd,wfd);
			close(infd);
			return 1;
		}
		inotify_rm_watch(infd,wfd);
		wfd = inotify_add_watch(infd,node,mode);
	}
	inotify_rm_watch(infd,wfd);
	close(infd);
	return 0;
}

void print_guenguencha(char* quien_soy){
	printf("@                                `:,,,:`                      @\n@                             ;'+;#++:',:                     @\n@                         ,;',++#'#+#'++;:`                   @\n@                         ;'+;++###+#+++++'.                  @\n@                   ,'+;';'##++++####+#+#++;:                 @\n@                   ;'+++,++'+########++++++'.                @\n@                  ;++;++;++####+##++++++++#'+                @\n@                ';'++###+++#######++++++++++++++#            @\n@                '+;+###+++++#################++++',          @\n@               '+####++++++###########################++     @\n@              ;++####++++######+++++++++++##############+    @\n@             ;'+#####++##########++++++++++++++###@######+#  @\n@             +'#####++##++++++++++++++++++++++####@#######+` @\n@            .'+#+####+''''''''+++++++++''+++######@######### @\n@            `++#+###'''''#######++''''+'''+++##+##@#######++ @\n@            .'+####''''@########+''''''''''''''+#@########+  @\n@            ;'++###'''@@@@@@@@####+''''''####+'''#@########+ @\n@            @+++###'''##++    ####''''''#######''#@########+ @\n@           `++#+#+#'''####+++###+++';'''#@@#@#@''@@@#######+ @\n@            '+#++#@'''''+######++'';;;''@@@   @@''@@######++ @\n@           @@@@@##@';;:;;''+++'''';::;''@@@@@@@@+@@@@#####+. @\n@           @@@@@#@;:::::;;''''''';;::;;;'+@@@@+++@@@@####++  @\n@          @@@@##@#:,,,::;;'''';;;::::;;;''++###+'@@@@####+,  @\n@          @@@@@@@':,,,::;;'''';;::::::;;;''++++''@@@@####+   @\n@          +@@@@@;,;::'#+'';;;;;;;;;::;;;::''++';:@#:         @\n@          '@@@@@;::::'++;;;;;;;;';;;;;;;;''''+';,'`          @\n@            @@@@:,:::'++#+':;;;'''''';;'''''++':;'           @\n@            @@@@,::;:'++##'#;;;;''''+'''';;;'#';;;           @\n@             @@@;::':'+++ '#::'''''''''';;' ++';';           @\n@              ;@+::;:;+';  +:,;''''''''''#  #';'',           @\n@                 :;::;+';  ';,;:''''''';    +':';:           @\n@                 :;:;:'';;   ;,;::::''''   #;;;+''           @\n@                 :::;:'';;;   ;;;::;;'    ______________     @\n@                 ::;;:;';;:             : \\              \\   @\n@                 :::;;;;;;;;          ;;:' \\   SOY UN     \\  @\n@              ::::::;;;::;;;;        ;;;;;  \\              \\ @\n@             ,:::::;;;;;::;;;;;     ;;;;:   /   %s  |@\n@            :,:::::;;;;;;:;;;;;'''''';;;   |                |@\n@  ######'::,::;::::;;;;;;;:;;;;;;;;;;;;:   |CHINGUENGUENCHA |@\n@#########;:,::;;:::;;;''';;;;;;;;;;;;;;    \\_______________/ @\n@@########+:,:;;;;;;;;;''''';;;;;;;;;;';';;'+##               @\n@@#@#######+,,;;;;;;;;''''''''''';;;;;;;:;:;+###              @\n@@@@@######@':;;;;;;;''''''''''++++;;;;::+;;+######           @\n@@@#@@#####@@';';;;;;;'''''''''''';;;;;:'##:+#########+       @\n@@@@@@@@@@@##@+;;;;;;;'''''''''''';;;;;:+@#'+###########`     @\n@@@@@@@@@@@@@@@#';;;;;'''''''''''';;;;;:@##++###########+     @\n@@@@@@@@@@@@@@@@@';;;'''''''''''';;;;;;:####+#############    @\n", quien_soy);

}

char *get_local_ip(void){
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	memcpy(ifr.ifr_name, "enp0s3", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	strcpy(LOCAL_IP,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	return LOCAL_IP;
}

Memoria *duplicar_memoria(Memoria *memoria){
	Memoria * duplicada = malloc(sizeof(Memoria));
	duplicada->idMemoria = memoria->idMemoria;
	duplicada->ip = malloc(strlen(memoria->ip) + 1);
	strcpy(duplicada->ip, memoria->ip);
	duplicada->puerto = malloc(strlen(memoria->puerto) + 1);
	strcpy(duplicada->puerto, memoria->puerto);
	return duplicada;
}

Retorno_Describe *duplicar_describe(Retorno_Describe *describe){
	Retorno_Describe *duplicado = malloc(sizeof(Retorno_Describe));
	duplicado->nombre_tabla = malloc(strlen(describe->nombre_tabla) + 1);
	strcpy(duplicado->nombre_tabla, describe->nombre_tabla);
	duplicado->consistencia = describe->consistencia;
	duplicado->particiones = describe->particiones;
	duplicado->compactation_time = describe->compactation_time;
	return duplicado;
}

t_list * list_duplicate_all(t_list *lista, void*(*duplicador)(void*), pthread_mutex_t mutex){
	t_list *duplicate = list_create();
	
	void duplicar(void *element){
		list_add(duplicate, duplicador(element));
	}
	pthread_mutex_lock(&mutex);
	list_iterate(lista, (void*)duplicar);
	pthread_mutex_unlock(&mutex);
	return duplicate;
}


void eliminar_describe(Retorno_Describe *ret_desc){
	free(ret_desc->nombre_tabla);
	free(ret_desc);
}

void eliminar_memoria(Memoria * memoria){
	free(memoria->ip);
	free(memoria->puerto);
	free(memoria);
}

void mostrar_memoria(Memoria * memoria){
	log_debug(LOG_DEBUG, "Memoria; %d, ip: %s, puerto: %s", memoria->idMemoria, memoria->ip, memoria->puerto);
}

char* config_get_string_value_check(t_config* config, char *key){
	char * value = config_get_string_value(config, key);

	if (value == NULL){
		log_error(LOG_ERROR,"No se encontro %s en %s.", key, PATH_CONFIG);
		exit_gracefully(EXIT_FAILURE);
	}

	return value;
}

int config_get_int_value_check(t_config* config, char *key){

	char * value = config_get_string_value(config, key);

	if (value == NULL){
		log_error(LOG_ERROR,"No se encontro %s en %s.", key, PATH_CONFIG);
		exit_gracefully(EXIT_FAILURE);
	}

	return atoi(value);
}

# -*-MakeFile-*-

FLAGS := -Wall -g

LIBS := -pthread -lcommons -lreadline

utilguenguencha_path := ../utilguenguencha/

all: default

default: clean dir poolmemory

general: dir poolmemory_general

.PHONY: dir
.SILENT: dir
dir:
	mkdir -p Release

.SILENT: poolmemory
poolmemory: src/*.* utilguenguencha
	echo "\nSe inicia compilacion del PoolMemory"
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/poolmemory
	echo "Se compilo correctamente el PoolMemory"

.SILENT: poolmemory_general
poolmemory_general: src/*.*
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/poolmemory

.PHONY: utilguenguencha
.SILENT: utilguenguencha
utilguenguencha:
	echo "\nSe inicia compilacion de Utilguenguenchas"
	make -sC $(utilguenguencha_path)
	echo "Se compilaron correctamente las Utilguenguenchas"

.SILENT: clean
clean:
	rm -rf Release Debug */logger.log
	echo "Se hizo clean PoolMemory"	

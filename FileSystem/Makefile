# -*-MakeFile-*-

FLAGS := -Wall -g

LIBS := -pthread -lcommons -lreadline

utilguenguencha_path := ../utilguenguencha/

all: default

default: clean dir fileSystem

general: dir fileSystem_general

.PHONY: dir
.SILENT: dir
dir:
	mkdir -p Release

.SILENT: fileSystem
fileSystem: src/*.* utilguenguencha
	echo "\nSe inicia compilacion del FileSystem"
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/fileSystem
	echo "Se compilo correctamente el FileSystem"

fileSystem_general: src/*.*
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/fileSystem

.PHONY: utilguenguencha
.SILENT: utilguenguencha
utilguenguencha:
	echo "\nSe inicia compilacion de Utilguenguenchas"
	make -sC $(utilguenguencha_path)
	echo "Se compilaron correctamente las Utilguenguenchas"

.SILENT: clean
clean:
	rm -rf Release Debug */logger.log
	echo "Se hizo clean FileSystem"

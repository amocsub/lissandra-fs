# -*-MakeFile-*-

FLAGS := -Wall -g

LIBS := -pthread -lcommons -lreadline

utilguenguencha_path := ../utilguenguencha/

all: default

default: clean dir kernel

general: dir kernel_general

.PHONY: dir
.SILENT: dir
dir:
	mkdir -p Release

.SILENT: kernel
kernel: src/*.* utilguenguencha
	echo "\nSe inicia compilacion del Kernel"
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/kernel
	echo "Se compilo correctamente el Kernel"

.SILENT: kernel_general
kernel_general: src/*.*
	gcc src/*.c $(utilguenguencha_path)Release/*.o $(FLAGS) $(LIBS) -o Release/kernel

.PHONY: utilguenguencha
.SILENT: utilguenguencha
utilguenguencha:
	echo "\nSe inicia compilacion de Utilguenguenchas"
	make -sC $(utilguenguencha_path)
	echo "Se compilaron correctamente las Utilguenguenchas"

.SILENT: clean
clean:
	rm -rf Release Debug */logger.log
	echo "Se hizo clean Kernel"	

# -*- MakeFile -*-

fileSystem_path := FileSystem/
kernel_path := Kernel/
poolMemory_path := PoolMemory/
utilguenguencha_path := utilguenguencha/

all: default

default: clean general

.PHONY: general
.SILENT: general
general:
	make -s utilguenguencha kernel mem fs
	echo "\nTe compilo todo el proyecto, que chinguenguencha!!"

.PHONY: fs
.SILENT: fs
fs:
	echo "\nSe inicia compilacion del FileSystem"
	make -sC $(fileSystem_path) general
	echo "Se compilo correctamente el FileSystem"

.PHONY: kernel
.SILENT: kernel
kernel:
	echo "\nSe inicia compilacion del Kernel"
	make -sC $(kernel_path) general
	echo "Se compilo correctamente el Kernel"

.PHONY: mem
.SILENT: mem
mem:
	echo "\nSe inicia compilacion del PoolMemory"
	make -sC $(poolMemory_path) general
	echo "Se compilo correctamente el PoolMemory"

.PHONY: utilguenguencha
.SILENT: utilguenguencha
utilguenguencha:
	echo "\nSe inicia compilacion de Utilguenguenchas"
	make -sC $(utilguenguencha_path)
	echo "Se compilaron correctamente las Utilguenguenchas"

.SILENT: clean
.PHONY: clean
clean:
	make -sC $(fileSystem_path) clean
	make -sC $(kernel_path) clean
	make -sC $(poolMemory_path) clean
	make -sC $(utilguenguencha_path) clean

#!/bin/bash
echo "----------------------------------"
echo "CHINGUENDEPLOY"
echo "----------------------------------"

case "$1" in 
 -entrega)
    echo "CONFIGURAR PARA 4 VM"

    echo "COMPILANDO REPO"
    make

    echo "ELIMINANDO DIRECTORIOS EXISTENTES"
    rm -r /home/utnso/lfs-base
    rm -r /home/utnso/lfs-prueba-kernel
    rm -r /home/utnso/lfs-prueba-memoria
    rm -r /home/utnso/lfs-compactacion
    rm -r /home/utnso/lfs-stress

    rm -r ./FileSystem/pruebas
    rm -r ./Kernel/pruebas
    rm -r ./PoolMemory/pruebas

    echo "CREANDO NUEVOS DIRECTORIOS"
    mkdir -p -v /home/utnso/lfs-base/Metadata
    mkdir -p -v /home/utnso/lfs-prueba-kernel/Metadata
    mkdir -p -v /home/utnso/lfs-prueba-memoria/Metadata
    mkdir -p -v /home/utnso/lfs-compactacion/Metadata
    mkdir -p -v /home/utnso/lfs-stress/Metadata

    mkdir -v ./FileSystem/pruebas
    mkdir -v ./Kernel/pruebas
    mkdir -v ./PoolMemory/pruebas

    echo "COPIANDO METADATAS"
    cp ./deploy/unasolavm/FileSystem/pruebas/base/Metadata.bin /home/utnso/lfs-base/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/kernel/Metadata.bin /home/utnso/lfs-prueba-kernel/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/memoria/Metadata.bin /home/utnso/lfs-prueba-memoria/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/lfs/Metadata.bin /home/utnso/lfs-compactacion/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/stress/Metadata.bin /home/utnso/lfs-stress/Metadata/Metadata.bin

    echo "Ingresar IP de VM1:"
    read vm1_ip

    echo "Ingresar IP de VM2:"
    read vm2_ip

    echo "Ingresar IP de VM3:"
    read vm3_ip

    echo "Ingresar IP de VM4:"
    read vm4_ip

    cp -r ./deploy/FileSystem/pruebas ./FileSystem/
    cp -r ./deploy/Kernel/pruebas ./Kernel/
    cp -r ./deploy/PoolMemory/pruebas ./PoolMemory/

    find ./Kernel/ -name "*.cfg" -print | xargs sed -i 's/VM1_IP/'$vm1_ip'/g'
    find ./Kernel/ -name "*.cfg" -print | xargs sed -i 's/VM2_IP/'$vm2_ip'/g'
    find ./Kernel/ -name "*.cfg" -print | xargs sed -i 's/VM3_IP/'$vm3_ip'/g'
    find ./Kernel/ -name "*.cfg" -print | xargs sed -i 's/VM4_IP/'$vm4_ip'/g'

    find ./PoolMemory/ -name "*.cfg" -print | xargs sed -i 's/VM1_IP/'$vm1_ip'/g'
    find ./PoolMemory/ -name "*.cfg" -print | xargs sed -i 's/VM2_IP/'$vm2_ip'/g'
    find ./PoolMemory/ -name "*.cfg" -print | xargs sed -i 's/VM3_IP/'$vm3_ip'/g'
    find ./PoolMemory/ -name "*.cfg" -print | xargs sed -i 's/VM4_IP/'$vm4_ip'/g'

    find ./FileSystem/ -name "*.cfg" -print | xargs sed -i 's/VM1_IP/'$vm1_ip'/g'
    find ./FileSystem/ -name "*.cfg" -print | xargs sed -i 's/VM2_IP/'$vm2_ip'/g'
    find ./FileSystem/ -name "*.cfg" -print | xargs sed -i 's/VM3_IP/'$vm3_ip'/g'
    find ./FileSystem/ -name "*.cfg" -print | xargs sed -i 's/VM4_IP/'$vm4_ip'/g'
    ;;
 -unavm)
    echo "CONFIGURAR PARA 1 VM"

    echo "COMPILANDO REPO"
    make

    echo "ELIMINANDO DIRECTORIOS EXISTENTES"
    rm -r /home/utnso/lfs-base
    rm -r /home/utnso/lfs-prueba-kernel
    rm -r /home/utnso/lfs-prueba-memoria
    rm -r /home/utnso/lfs-compactacion
    rm -r /home/utnso/lfs-stress

    rm -r ./FileSystem/pruebas
    rm -r ./Kernel/pruebas
    rm -r ./PoolMemory/pruebas

    echo "CREANDO NUEVOS DIRECTORIOS"
    mkdir -p -v /home/utnso/lfs-base/Metadata
    mkdir -p -v /home/utnso/lfs-prueba-kernel/Metadata
    mkdir -p -v /home/utnso/lfs-prueba-memoria/Metadata
    mkdir -p -v /home/utnso/lfs-compactacion/Metadata
    mkdir -p -v /home/utnso/lfs-stress/Metadata

    mkdir -v ./FileSystem/pruebas
    mkdir -v ./Kernel/pruebas
    mkdir -v ./PoolMemory/pruebas

    echo "COPIANDO METADATAS"
    cp ./deploy/unasolavm/FileSystem/pruebas/base/Metadata.bin /home/utnso/lfs-base/Metadata/Metadata.bin
    cp ./\deploy/unasolavm/FileSystem/pruebas/kernel/Metadata.bin /home/utnso/lfs-prueba-kernel/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/memoria/Metadata.bin /home/utnso/lfs-prueba-memoria/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/lfs/Metadata.bin /home/utnso/lfs-compactacion/Metadata/Metadata.bin
    cp ./deploy/unasolavm/FileSystem/pruebas/stress/Metadata.bin /home/utnso/lfs-stress/Metadata/Metadata.bin

    cp -r ./deploy/unasolavm/FileSystem/pruebas ./FileSystem/
    cp -r ./deploy/unasolavm/Kernel/pruebas ./Kernel/
    cp -r ./deploy/unasolavm/PoolMemory/pruebas ./PoolMemory/
    ;;
 -clean)
    echo "ELIMINANDO DIRECTORIOS EXISTENTES"
    rm -r /home/utnso/lfs-base
    rm -r /home/utnso/lfs-prueba-kernel
    rm -r /home/utnso/lfs-prueba-memoria
    rm -r /home/utnso/lfs-compactacion
    rm -r /home/utnso/lfs-stress

    rm -r ./FileSystem/pruebas
    rm -r ./Kernel/pruebas
    rm -r ./PoolMemory/pruebas
    ;;
*)
    echo "No reconozco el argumento"

    echo "-clean: Elimina todos los directorios"
    echo "-unavm: Realiza la configuracion para probar todo en una sola VM"
    echo "-entrega: Realiza la configuracion para probar en 4 VM"
    ;;
esac

echo "Listo el pollo"

exit
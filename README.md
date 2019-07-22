# TP SISTEMAS OPERATIVOS 2019/1C
Se presenta la solución propuesta del grupo "Lo Compilaste Todo Chinguenguencha" para el TP de Sistemas Operativos en la UTN-FRBA para el 1C 2019.

[Enunciado del TP](https://docs.google.com/document/d/1QlzXwpSvI5ua2lbO8pF6ZgjlgMndFlwzlAci7qhZmqE/edit)

## DEPLOYMENT 🔧

### Entrega en laboratorio con 4 VM
_Entramos en la carpeta del repositorio y ejecutamos el bash deplot.sh con el argumento -entrega._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-
- sh deploy.sh -entrega
```
_A continuacion se eliminarán los directorios que debemos utilizar en caso de que existan previamente._
_Se crearán los directorios necesarios._
```
--directorios necesarios para el FileSystem
/home/utnso/lfs-base/Metadata
/home/utnso/lfs-prueba-kernel/Metadata
/home/utnso/lfs-prueba-memoria/Metadata
/home/utnso/lfs-compactacion/Metadata
/home/utnso/lfs-stress/Metadata

--directorios que contendran los config para cada prueba, para cada proceso
./FileSystem/pruebas
./Kernel/pruebas
./PoolMemory/pruebas
```
_Se copiarán los archivos Metadata.bin para cada prueba._
_El bash nos solicitará ingresar las IPs de las 4 VM que vamos a utilizar._
_Se reemplazar las IPs ingresadas en todos los config y se copiaran a los directorios de /pruebas de cada proceso._

### Para probar en una sola VM
_Entramos en la carpeta del repositorio y ejecutamos el bash deplot.sh con el argumento -unavm._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-
- sh deploy.sh -unavm
```

_A continuacion se eliminarán los directorios que debemos utilizar en caso de que existan previamente._
_Se crearán los directorios necesarios._
_Se copiarán los archivos Metadata.bin para cada prueba._
_Se modificaran los config.cfg con la IP 127.0.0.1 utilizando puertos diferentes y se copiaran a los directorios de /pruebas de cada proceso._

### Para eliminar los directorios creados
_Entramos en la carpeta del repositorio y ejecutamos el bash deplot.sh con el argumento -clean._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-
- sh deploy.sh -clean
```
_A continuacion se eliminarán los directorios creados por deployments previos._

### La secuencia de compilación debería ser la siguiente:
```
- Se hizo clean FileSystem
- Se hizo clean Kernel
- Se hizo clean PoolMemory
- Se hizo clean de las Utilguenguenchas
-   
- Se inicia compilacion de Utilguenguenchas
- Se hizo clean de las Utilguenguenchas
-    Se compilo correctamente comunicacion
-    Se compilo correctamente parser
-    Se compilo correctamente utils
- Se compilaron correctamente las Utilguenguenchas
-   
- Se inicia compilacion del Kernel
- Se compilo correctamente el Kernel
-   
- Se inicia compilacion del PoolMemory
- Se compilo correctamente el PoolMemory
-  
- Se inicia compilacion del FileSystem
- Se compilo correctamente el FileSystem
-  
- Te compilo todo el proyecto, que chinguenguencha!!
```

## Ejecutando las pruebas ⚙️
[Enunciado pruebas](https://docs.google.com/document/d/1m_V2AXpfo8SpeOr330Rwj3uKIe-GHJ3VNfO38FNNc6Q/edit#)

[Repositorio pruebas](https://github.com/sisoputnfrba/1C2019-Scripts-lql-entrega)

### Prueba Base
_Primero ejecutar el FileSsystem._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/FileSystem
- ./Release/fileSystem pruebas/base/config_filesystem.cfg
```
_Ejecutar las Memorias._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/base/config_memoria1.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/base/config_memoria2.cfg
```
_Ejecutar el Kernel._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/Kernel
- ./Release/kernel pruebas/base/config_kernel.cfg
```

_Para ejecutar los scripts lql en la consola del Kernel_
```
>> run pruebas/base/simple_ec.lql
>> run pruebas/base/simple_sc.lql
>> run pruebas/base/simple_shc.lql
```

### Prueba Kernel
_Primero ejecutar el FileSsystem._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/FileSystem
- ./Release/fileSystem pruebas/kernel/config_filesystem.cfg
```
_Ejecutar las Memorias._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/kernel/config_memoria1.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/kernel/config_memoria2.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/kernel/config_memoria3.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/kernel/config_memoria4.cfg
```
_Ejecutar el Kernel._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/Kernel
- ./Release/kernel pruebas/kernel/config_kernel.cfg
```
_Para ejecutar los scripts lql en la consola del Kernel_
```
>> run pruebas/kernel/animales.lql
>> run pruebas/kernel/comidas.lql
>> run pruebas/kernel/cosas_falla.lql
>> run pruebas/kernel/internet_browser_falla.lql
>> run pruebas/kernel/misc_1.lql
>> run pruebas/kernel/misc_2.lql
```

### Prueba Memoria
_Primero ejecutar el FileSsystem._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/FileSystem
- ./Release/fileSystem pruebas/memoria/config_filesystem.cfg
```
_Ejecutar las Memorias._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/memoria/config_memoria1.cfg
```
_Ejecutar el Kernel._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/Kernel
- ./Release/kernel pruebas/memoria/config_kernel.cfg
```

_Para ejecutar los scripts lql en la consola del Kernel_
```
>> run pruebas/memoria/reemplazo1.lql
>> run pruebas/memoria/reemplazo2.lql
```

### Prueba Compactacion
_Primero ejecutar el FileSsystem._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/FileSystem
- ./Release/fileSystem pruebas/lfs/config_filesystem.cfg
```
_Ejecutar las Memorias._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/lfs/config_memoria1.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/lfs/config_memoria2.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/lfs/config_memoria3.cfg
```
_Ejecutar el Kernel._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/Kernel
- ./Release/kernel pruebas/lfs/config_kernel.cfg
```

_Para ejecutar los scripts lql en la consola del Kernel_
```
>> run pruebas/lfs/compactacion_larga.lql
```

### Prueba Stress
_Primero ejecutar el FileSsystem._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/FileSystem
- ./Release/fileSystem pruebas/stress/config_filesystem.cfg
```
_Ejecutar las Memorias._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/stress/config_memoria1.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/stress/config_memoria2.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/stress/config_memoria3.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/stress/config_memoria4.cfg
```
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/PoolMemory
- ./Release/poolMemory pruebas/stress/config_memoria5.cfg
```
_Ejecutar el Kernel._
```
- cd tp-2019-1c-Lo-Compilaste-todo-Chinguenguencha-/Kernel
- ./Release/kernel pruebas/stress/config_kernel.cfg
```

_Para ejecutar los scripts lql en la consola del Kernel_
```
>> run pruebas/stress/cities_countries.lql
>> run pruebas/stress/cosas_falla.lql
>> run pruebas/stress/games_computer.lql
>> run pruebas/stress/internet_browser_falla.lql
>> run pruebas/stress/library_study.lql
>> run pruebas/stress/nintendo_playstation.lql
```
## RESULTADOS 📋
_Para mostrar los resultados, cada proceso utiliza los siguientes archivos de log:_
```
LOG_OUTPUT.log //muestra los resultados para las consultas hechas desde consola.
LOG_OUTPUT_SV.log //muestra los resultados para las consultas hechas por la API.
LOG_ERROR.log //muestra los errores para las consultas hechas desde consola.
LOG_ERROR_SV.log //muestra los errores para las consultas hechas por la API.
LOG_INFO.log  //muestra informacion sobre la ejecucion del proceso.
LOG_METRICS.log // (Solo para Kernel) muestra las metricas calculadas cada 30 segundos.
```

## Integrantes ✒️

* **Busco Matias** - *K3053*
* **Gianotti Lucas** - *K3053*
* **Tomasone Matias** - *K3054*
* **Aceval Alejo** - *K3051*



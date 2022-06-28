# Descripción comportamiento repositorio:

Hacer una busqueda en menos de 2 segundos que utilice como máximo 1MB de memoria RAM para un archivo .csv de casi 6 millones de registros

# Integrantes:

Brian Chaparro Cetina
bchaparro@unal.edu.co

Joe Zafir Méndez León
jzmendezl@unal.edu.co

Juan Sebastian Bautista Suárez
jbautistas@unal.edu.co

# Proceso para ejecución de todo el programa:

Luego de descargar todo el código:

1. Se debe guardar el archivo csvfinal.csv en el directorio toCreation que se encuentra:

    - https://bit.ly/3s1WVmT

    El archivo .csv que se encuentra en el link anterior es el mismo que se encuentra en el siguiente link:

    - https://bit.ly/37Jvqrz

    La diferencia es que el primero no tiene título y un salto de línea que se encuentra después del último registro, estas dos características sí se encuentran en el segundo archivo .csv. Estas dos pequeñas variaciones se hacen para no ejecutar líneas de código que eviten estas dos características.

2. Ingresar a toCreation y ejecutar:

    - 1.c (duración de ejecución es menos de 10 segundos), 
    - 2.c (duración de ejecución es al instante) 
    - 3.c (duración de ejecución es menos de 40 segundos)
    
    Estos son los valores esperados, puede que varíe dependiendo de cada computador.

    Estos tres programas crean dos archivos binarios llamados data.bin y hashtable.bin que quedan guardados y dilegencias correctamente en toQuery que permite hacer la busqueda con las condiciones dadas en la sección "Descripción comportamiento repositorio".

3. Ingresar a toQuery y ejecutar interface.c (que es el programa principal que ejecuta la interfaz para el usuario), este ejecuta find.c exactamente después de obtener los valores adecuados por parte del usuario.
    1. El archivo binario llamado share.bin es el que se utiliza entre interface.c y find.c para hacer shared memory
    2. memoryTest.c es un script que se agrega intencionalmente para comprobar que un script muy sencillo como este utiliza casi 0,6 MB (usando el comando top de BashShell se obtiene este resultado), que es excesivo para un script con pocas características. El "Monitor del sistema" en Ubuntu muestra valores más pequeños en MB, se recomienda hacer el cálculo de memoria RAM con ambas herramientas y analizar cuál les parecería más conveniente para llegar al valor más exacto en cuanto a memoria RAM.
     
# Cálculo de memoria RAM y tiempo para verificación:

1. Para comprobar el uso de memoria RAM menor a 1 MB seguir las indicaciones del inciso 3.2 de "Proceso para ejecución de todo el programa" ejecutando sólo find.c después de ejecutar de manera completa las indicaciones que están en el inciso 3.0 de la misma sección (sección anterior) porque de esta manera queda guardado correctamente el sourceid, dstid y hod de manera binaria en share.bin que después usa find.c para ejecutar su proceso correctamente.

    No hacer el cálculo con el programa interface.c porque este consume memoria adicional que está asociada a la interface y la condición de consumir menos de 1MB es asociada sólo a la busqueda.

2. Para comprobar la rapidez de la busqueda primero se debe de ejecutar de manera completa las indicaciones que están en el inciso 3.0 de "Proceso para ejecución de todo el programa", porque de esta manera queda guardado correctamente el sourceid, dstid y hod de manera binaria en share.bin que después usa find.c para ejecutar su proceso correctamente. Por último ejecutar el comando:

    "time ./find"

    O usar:

    "time make scriptfind"

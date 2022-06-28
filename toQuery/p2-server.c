#include<stdio.h>
#include<stdlib.h>
#include"../header.h"
#include <sys/types.h> //Para ejecutar socket() junto con la siguiente librería
#include <sys/socket.h>
#include <netinet/in.h> //Para usar sockaddr_in
#include <strings.h> //Para ejecutar bzero()
#include <unistd.h> //Para ejecutar close()
#include <arpa/inet.h> //Para ejecutar inet_ntop()
#include <pthread.h> //Para ejecutar todo lo que tiene que ver con threads

#define NUM_HILOS 3 //Este número se usa también para el BACKLOG en la función listen()
#define PORT 3535   //Puerto por defecto para la comunicación entre server y client

struct server_information{
    /*
    Struct para enviar la información del server a cada hilo para luego usarla en la
    función connect(). Este struct se envía desde la función threading a accepting
    */
    int serverfd, addrlen;
    struct sockaddr_in server;
    struct sharerow sr1;
};

int threading(int serverfd,int addrlen, struct sockaddr_in server); //Función para lanzar todos los hilos

int * accepting(void * server_information); //Función para esperar la conexión de un client desde cada hilo

int searching(struct sharerow sr1, int clientfd); //Función para hacer búsqueda en el archivo de todos los viajes de UBER para luego enviarla a cada client

//void loggin(void * str, struct tm *tm, struct row *r1);

void logging(int flag, struct row *r1);

int main(){
    int serverfd; //Declarando descriptor de server
    struct sockaddr_in server; //Struct para configurar el server
    int r,opt=1; //r es para manejar los errores y opt para usarlo en función setsockopt
    int addrlen = sizeof(server); //Para obtener el tamaño del struct sockaddr_in
    
    serverfd = socket(AF_INET,SOCK_STREAM,0); //Declarando el server
    if(serverfd<0){
        perror("Error en socket()\n"); //Manejando los errores de socket(()
        exit(0);
    }

    r = setsockopt(serverfd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); //Para poder usar el mismo puerto 3535 nuevamente
    if(r < 0){
        perror("Error en setsockopt()\n"); //Manejando los errores de setsockopt()
        exit(0);
    }

    //Las siguientes cuatro líneas son para inicializar atributos que necesita el server para ser configurado
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero,0);

    r = bind(serverfd,(struct sockaddr *)&server, sizeof(server)); //Nombrando el server
    if(r<0){
        perror("Error en bind()\n"); //Manejando los errores de bind()
        exit(0);
    }
    
    r = listen(serverfd,NUM_HILOS); //Configurando el server
    if(r<0){
        perror("Error en listen()\n"); //Manejando los errores de listen()
        exit(0);
    }

    threading(serverfd,addrlen,server); //Ejecutando la cantidad de hilos que es igual a NUM_HILOS

    close(serverfd); //Cerrando el server lo de ejecutarse todo el proceso
    printf("El server terminó la ejecución!\n");
}

int threading(int serverfd,int addrlen, struct sockaddr_in server){
    /*
    Función para lanzar todos los hilos
    */
    pthread_t hilo[NUM_HILOS]; //Declarando todos los hilos y almacenándolos en una arreglo
    struct server_information inf1[NUM_HILOS]; //Declarando todos los struct server_information y almacenadolos en un arreglo para que luego sean utilizados por cada hilo
    int *rh0; //Para ser utilizados en la función pthread_join()

    for(int i = 0; i < NUM_HILOS; i++){ //Inicializando todos los struct server_information almacenados anteriormente en inf1
        inf1[i].serverfd = serverfd ;
        inf1[i].addrlen = addrlen;
        inf1[i].server = server;
        struct sharerow sr1;
        inf1[i].sr1 = sr1;
    }

    for(int i=0; i<NUM_HILOS; i++){
        int r = pthread_create(&hilo[i], NULL, (void *)accepting, (void *)&inf1[i]); //Inicializando todos los hilos
        if(r !=0){
            perror("\n-->pthread_create erro: "); //Manejando los errores de pthread_create()
            exit(0);
        }
    }

    for(int i=0; i<NUM_HILOS; i++){
        int r = pthread_join(hilo[i], (void **)&rh0); //Esperando que todos los hilos terminen su proceso para dejar de ejecutar el proceso "PADRE", que es el servidor
        if(r!=0){
            perror("\n-->pthread_join error: "); //Manejando los errores de pthread_join()
            exit(0);
        }
    }

    printf("\n");
    return 0;
}

int * accepting(void * server_information){
    /*
    Función para esperar la conexión de un client desde cada hilo
    */
    int clientfd,r; //Clientfd es el descriptor del cliente y r se utiliza para manejar errores en toda la función
    struct server_information *inf1 = server_information; //Almacenando cada struct server_information en inf1 como un apuntador
    struct sharerow sr1 = inf1->sr1; //Almacenando el struct sharerow, que se encuentra en el struct server_information, en sr1
    
    clientfd = accept(inf1->serverfd,(struct sockaddr *)&inf1->server, (socklen_t*)&inf1->addrlen); //Para esperar que un cliente se conecte
    if(clientfd<0){
        perror("Error en accept()\n"); //Manejando los errores de accept()
        exit(0);
    }
   
    r = recv(clientfd,&sr1,sizeof(struct sharerow),0); //Para recibir la información que envía el client
    if(r<0){
        perror("Error en recv en server\n"); //Manejando los errores de recv()
        exit(0);
    }

    //Las siguientes cuatro líneas de código son para poder obtener la dirección ip del client
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&inf1->server.sin_addr;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

    //Las siguientes dos líneas de código son para poder obtener la hora exacta en el que el server recibe la información del client
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    //Las siguientes 10 línes de código son para hacer la primera parte del log, es decir, guardar la ip y la hora de conexión
    FILE *flog;
    flog = fopen("log.txt","a");
    if(flog==NULL){
        printf("Hubo un error con el opening del log.text\n");
        exit(0);
    }
    fseek(flog,0,SEEK_END);
    fprintf(flog,"[Fecha %d-%02d-%02d %02d:%02d:%02d] Cliente [%s] ",\
    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,str);
    fclose(flog);

    searching(sr1,clientfd); //Haciendo la busqueda en el archivo de casi seis millones de registros de viajes de UBER
    close(clientfd); //Cerrando el client luego de hacer el searching
}

int searching(struct sharerow sr1, int clientfd){
    /*
    Función para hacer búsqueda en el archivo de todos los viajes de UBER para luego enviarla a cada client
    */
    int *np=malloc(sizeof(int)); //Esta variable guardará la posición del registro, a leer o leído, del archivo data.bin de los viajes de UBER y así se sabrá cuándo se llega al final del archivo
    struct row r1; //Struct para hacer busquedas en el archivo de los viajes de UBER llamado data.bin. La definición de este struct está en ../header.h
    struct hashrow hr1; //Struct para hacer busquedas en la pseduo hashtable que se almacena en hashtable.bin. La definición de este struct está en ../header.h

    if(sr1.sourceid == 0 || sr1.dstid == 0){ //Para parar el proceso si los datos obtenidos en sourceid o dstid fueron igual a cero
        r1.isfound = 0; //Inicializando el atributo isfound en 0 para que cuando se envíe la información al client sepa que no se encontró ningún registro 
        int r = send(clientfd,&r1,sizeof(struct row),0); //Enviando la información obtenida, hasta este punto, al client 
        if(r<0){
            perror("Error en send()\n"); //Manejando los errores de send()
            exit(0);
        }
        logging(0,&r1); //Para hacer la segunda parte del log, es decir, para poner [búsqueda - origen - destino]
        return 0; //Aquí no se coloca exit() porque eso mata el proceso PADRE, o server, que declara todos los hilos
    }

    FILE *filehash;
    filehash = fopen("hashtable.bin","r+b"); //Inicializando el puntero para el archivo hashtable.bin
    if(filehash==NULL){
        printf("Ocurrió un problema con el opening del FILE hashtable.bin"); //Manejando los errores de fopen()
        exit(0);
    }

    FILE *filedata;
    filedata = fopen("data.bin","r+b"); //Inicializando el puntero para el archivo data.bin
    if(filedata==NULL){
        printf("Ocurrió un problema con el opening del FILE data.bin"); //Manejando los errores de fopen()
        exit(0);
    }
    
    fseek(filehash,(sr1.sourceid-1)*sizeof(struct hashrow),SEEK_SET); //Buscando en el archivo hashtable.bin en la posición almacenada en sourceid
    fread(&hr1,sizeof(struct hashrow),1,filehash); //Leyendo el valor que está en la posición sourceid del archivo hashtable.bin y almacenándolo en el struct hr1
    fclose(filehash); //Cerrando el puntero del archivo hashtable.bin

    *np = hr1.img;//El valor para buscar ahora en data.bin
    
    short int flag = 0; //Para manejar el flujo del siguiente ciclo while
    while(*np!=0){
        fseek(filedata,(*np-1)*sizeof(struct row),SEEK_SET); //Empezando la busqueda en el archivo data.bin en la posición almacenada en el struct hr1.img
        fread(&r1,sizeof(struct row),1,filedata); //Leyendo el registro que está en la posición hr1.img del archivo data.bin
        if(sr1.sourceid==r1.sourceid && sr1.dstid==r1.dstid && sr1.hod==r1.hod){ //Comprando si en hr1.img están los tres valores datos por el usuario
            fclose(filedata); //Cerrando el puntero del archivo data.bin cuando se entra al if
            free(np); //Liberando la memoria de la variable np que se definió al principio de esta función y fue necesaria para saber cuándo se llega el final del archivo data.bin
            r1.isfound = 1; //Inicializando el atributo isfound en 1 para que cuando se envíe la información al client sepa que se encontró un registro 
            int r = send(clientfd,&r1,sizeof(struct row),0); //Enviando la información obtenida en la posición del archivo data.bin donde hay coincidencia
            if(r<0){
                perror("Error en send()\n"); //Manejando los errores de send()
                exit(0);
            }
            flag = 1; //Cambiando el valor de la variable flag para que no ejecute el if que está después de este ciclo
            logging(1,&r1); //Para hacer la segunda parte del log, es decir, para poner [búsqueda - origen - destino]
            break;
        }
        *np=r1.npos;
    }
    if(flag == 0){ //En caso de no encontrar ninguna coincidencia en el ciclo anterior se ejecutan las siguientes cuatro líneas
        r1.isfound = 0; //Inicializando el atributo isfound en 0 para que cuando se envíe la información al client sepa que no se encontró ningún registro 
        int r = send(clientfd,&r1,sizeof(struct row),0); //Enviando la información obtenida, hasta este punto, al client 
        if(r<0){
            perror("Error en send()\n"); //Manejando los errores de send()
            exit(0);
        }
        fclose(filedata); // Se cierra el puntero del archivo data.bin
        free(np); //Se libera la memoria de la variabel np usada para guardar la posición del registro leído en el archivo data.bin
        logging(0,&r1); //Para hacer la segunda parte del log, es decir, para poner [búsqueda - origen - destino]
    }
    return 0;
}



void logging(int flag, struct row *r1){
    FILE *flog; //Puntero del archivo log.txt
    flog = fopen("log.txt","a");
    if(flog==NULL){
        printf("Hubo un error con el opening del log.text\n");
        exit(0);
    }

    if(flag==1){//Bloque de código por si se encontró alguna coincidencia en el archivo data.bin
        fseek(flog,0,SEEK_END);
        fprintf(flog,"[busqueda: %f - origen: %d - destino: %d]\n",r1->mean, r1->sourceid, r1->dstid);
        fclose(flog);
    }else{//Bloque de código por si no se encontró alguna coincidencia en el archivo data.bin
        fseek(flog,0,SEEK_END);
        fprintf(flog,"[busqueda: %s - origen: %s - destino: %s]\n","N/A","N/A","N/A");
        fclose(flog);
    }
    
}


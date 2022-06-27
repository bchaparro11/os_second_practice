#include<stdio.h>
#include<stdlib.h>
#include"../header.h"
#include <sys/types.h> //Para ejecutar socket() junto con la siguiente librería
#include <sys/socket.h>
#include <netinet/in.h> //Para usar sockaddr_in
#include <strings.h> //Para ejecutar bzero()
//#include <string.h>
#include <unistd.h> //Para ejecutar close()
//#include <netdb.h>
#include <arpa/inet.h> //Para ejecutar inet_ntop()
#include <pthread.h> //Para ejecutar todo lo que tiene que ver con threads

#define NUM_HILOS 1 //Este número se usa también para el BACKLOG en la función listen()
#define PORT 3535   //Puerto por defecto para la comunicación entre server y client

struct server_information{
    int serverfd, addrlen;
    struct sockaddr_in server;
    struct sharerow sr1;
};

int threading(int serverfd,int addrlen, struct sockaddr_in server);

int * connecting(void * server_information);

int searching(struct sharerow sr1, int clientfd);

int main(){

    int serverfd;
    struct sockaddr_in server;
    int r,opt=1;
    int addrlen = sizeof(server);
    
    serverfd = socket(AF_INET,SOCK_STREAM,0);
    if(serverfd<0){
        perror("Error en socket()\n");
        exit(0);
    }

    r = setsockopt(serverfd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if(r < 0){
        perror("Error en setsockopt()\n");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero,0);

    r = bind(serverfd,(struct sockaddr *)&server, sizeof(server));
    if(r<0){
        perror("Error en bind()\n");
        exit(0);
    }
    
    r = listen(serverfd,NUM_HILOS);
    if(r<0){
        perror("Error en listen()\n");
        exit(0);
    }

    threading(serverfd,addrlen,server);

    close(serverfd);
}

int threading(int serverfd,int addrlen, struct sockaddr_in server){

    pthread_t hilo[NUM_HILOS];
    struct server_information inf1[NUM_HILOS];
    int *rh0, i;

    for(int i = 0; i < NUM_HILOS; i++){
        inf1[i].serverfd = serverfd ;
        inf1[i].addrlen = addrlen;
        inf1[i].server = server;
        struct sharerow sr1;
        inf1[i].sr1 = sr1;
    }

    for(int i=0; i<NUM_HILOS; i++){
        int r = pthread_create(&hilo[i], NULL, (void *)connecting, (void *)&inf1[i]);
        if(r !=0){
            perror("\n-->pthread_create erro: ");
            exit(0);
        }
    }

    for(int i=0; i<NUM_HILOS; i++){
        int r = pthread_join(hilo[i], (void **)&rh0);
        if(r!=0){
            perror("\n-->pthread_join error: ");
            exit(0);
        }
    }

    printf("\n");
    return 0;
}

int * connecting(void * server_information){

    int clientfd,r;
    struct server_information *inf1 = server_information;
    struct sharerow sr1 = inf1->sr1;
    
    clientfd = accept(inf1->serverfd,(struct sockaddr *)&inf1->server, (socklen_t*)&inf1->addrlen);
    if(clientfd<0){
        perror("Error en accept()\n");
        exit(0);
    }

    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&inf1->server.sin_addr;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

    printf("La dirección ip es: %s\n",str);
   
    r = recv(clientfd,&sr1,sizeof(struct sharerow),0);
    if(r<0){
        perror("Error en recv en server\n");
        exit(0);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    printf("a: %d, b: %d, c: %d\n",sr1.sourceid,sr1.dstid,sr1.hod);
    
    searching(sr1,clientfd);
    
    close(clientfd);

}

int searching(struct sharerow sr1, int clientfd){

    int *np=malloc(sizeof(int));
 
    //Si no se escriben datos en interface.bin no es necesario seguir con el código más adelante
    if(sr1.sourceid == 0 || sr1.dstid == 0){
        printf("N/A\n");
        exit(0);
    }

    struct row r1;
    struct hashrow hr1;

    FILE *filehash;
    filehash = fopen("hashtable.bin","r+b");
    if(filehash==NULL){
        printf("Ocurrió un problema con el opening del FILE hashtable.bin");
        exit(0);
    }

    FILE *filedata;
    filedata = fopen("data.bin","r+b");
    if(filedata==NULL){
        printf("Ocurrió un problema con el opening del FILE data.bin");
        exit(0);
    }
    

    fseek(filehash,(sr1.sourceid-1)*sizeof(struct hashrow),SEEK_SET);
    fread(&hr1,sizeof(struct hashrow),1,filehash);
    fclose(filehash);

    //El valor para buscar ahora en data.bin
    *np = hr1.img;
    
    short int forWait,flag = 0;
    while(*np!=0){
        fseek(filedata,(*np-1)*sizeof(struct row),SEEK_SET);
        fread(&r1,sizeof(struct row),1,filedata);
        if(sr1.sourceid==r1.sourceid && sr1.dstid==r1.dstid && sr1.hod==r1.hod){
            fclose(filedata);
            free(np);
            //Para la prueba de tiempo no debe estar activo las siguientes dos líneas
            //printf("\nEsperando el ingreso de cualquier número para terminar el proceso: ");
            //scanf("%hd",&forWait);
            int r = send(clientfd,&r1,sizeof(struct row),0);
            if(r<0){
                perror("Error en send()\n");
                exit(0);
            }

            //close(clientfd);
            flag = 1;
            break;
            //exit(0);
        }
        *np=r1.npos;
    }
    if(flag == 0){
        printf("NA\n");
        fclose(filedata);
        free(np);
    }
    
    //Para la prueba de tiempo no debe estar activo las siguientes dos líneas
    //printf("\nEsperando el ingreso de cualquier número para terminar el proceso: ");
    //scanf("%hd",&forWait);
    return 0;
}
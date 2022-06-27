#include<stdio.h>
#include<stdlib.h>
#include"../header.h"







//to do socket()
#include <sys/types.h>
#include <sys/socket.h>
//to do struct
#include <netinet/in.h>
//to do bzero() connecting
#include <strings.h>
#include <string.h>
//to listen()
#define BACKLOG 8
//to do close()
#include <unistd.h>

//additional
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM_HILOS 3


struct server_information{
    int serverfd, addrlen;
    struct sockaddr_in server;
    struct sharerow sr1;
};

int * connecting(void * server_information){
    int clientfd,r;
    struct server_information *inf1 = server_information;
    struct sharerow sr1 = inf1->sr1;
    //clientfd = accept(serverfd,(struct sockaddr *)&client, &socklen);
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
    //Se debe poner el número para que funcione 
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

    /*
    struct sharerow sr2;
    sr2.sourceid = sr1.sourceid + 1;
    sr2.dstid=sr1.dstid + 1;
    sr2.hod=sr1.hod + 1;

    printf("a: %d, b: %d, c: %d\n",sr2.sourceid,sr2.dstid,sr2.hod);
    */

    
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

int threading(int serverfd,int addrlen, struct sockaddr_in server){
    pthread_t hilo[NUM_HILOS];
    struct server_information datosh[NUM_HILOS];
    int *rh0, i;

    for(int i = 0; i < NUM_HILOS; i++){
        datosh[i].serverfd = serverfd ;
        datosh[i].addrlen = addrlen;
        datosh[i].server = server;
        struct sharerow sr1;
        datosh[i].sr1 = sr1;
    }

    for(int i=0; i<NUM_HILOS; i++){
        int r = pthread_create(&hilo[i], NULL, (void *)connecting, (void *)&datosh[i]);
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

int main(){
    //struct sharerow sr1;

    int serverfd, clientfd;
    struct sockaddr_in server, client;
    int r,opt=1;
    //socklen_t socklen;
    int addrlen = sizeof(server);
    char buffer[1024] = { 0 };

    //Aparece un error pero en realidad no hay ningún error con el AF_INET
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
    server.sin_port = htons(3535);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero,0);


    // tener cuidado por si toca hacer casting o no en el tercer argumento
    //r = bind(serverfd,(struct sockaddr *)&server, sizeof(struct sockaddr));
    r = bind(serverfd,(struct sockaddr *)&server, sizeof(server));
    if(r<0){
        perror("Error en bind()\n");
        exit(0);
    }
    
    r = listen(serverfd,BACKLOG);
    if(r<0){
        perror("Error en listen()\n");
        exit(0);
    }


    //-----------------------------------------------------------------

    threading(serverfd,addrlen,server);
    






    

    //printf("%s\n", buffer);
    //printf("a: %d, b: %d, c: %d\n",sr1->sourceid,sr1->dstid,sr1->hod);

    /*
    r =  send(clientfd,"hola mundo",10,0);
    if(r<0){
        perror("Error en send()\n");
        exit(0);
    }
    */
    

    //close(clientfd);
    close(serverfd);
}

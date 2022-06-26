#include <stdio.h>
#include <stdlib.h>
//to do socket()
#include <sys/types.h>
#include <sys/socket.h>
//to do struct
#include <netinet/in.h>
//to do bzero() function
#include <strings.h>
#include <string.h>
//to listen()
#define BACKLOG 8
//to do close()
#include <unistd.h>

//additional
#include <netdb.h>
#include <arpa/inet.h>

struct row{
    int a,b,c;
};

int main(){

    struct row r1;

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

    //clientfd = accept(serverfd,(struct sockaddr *)&client, &socklen);
    clientfd = accept(serverfd,(struct sockaddr *)&server, (socklen_t*)&addrlen);
    if(clientfd<0){
        perror("Error en accept()\n");
        exit(0);
    }
    //Se debe poner el número para que funcione 
    r = recv(clientfd,&r1,sizeof(struct row),0);
    if(r<0){
        perror("Error en recv en server\n");
        exit(0);
    }

    //printf("%s\n", buffer);
    printf("a: %d, b: %d, c: %d\n",r1.a,r1.b,r1.c);
    r =  send(clientfd,"hola mundo",10,0);
    if(r<0){
        perror("Error en send()\n");
        exit(0);
    }

    close(clientfd);
    close(serverfd);
}
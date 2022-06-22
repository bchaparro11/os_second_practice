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


int main(){
    int serverfd, clientfd;
    struct sockaddr_in server, client;
    int r,opt=1;
    socklen_t socklen;
    int addrlen = sizeof(server);

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

    r =  send(clientfd,"hola mundo",10,0);
    if(r<0){
        perror("Error en send()\n");
        exit(0);
    }

    close(clientfd);
    close(serverfd);
}
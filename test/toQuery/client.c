#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
//to do struct
#include <netinet/in.h>
//to do inet_aton()
#include <netinet/in.h>
#include <arpa/inet.h>
//to do close()
#include <unistd.h>
//additional
#include <netdb.h>

#include <string.h>

//defines
#define PORT 3535

struct client_server_comunication{
    int a,b,c;
};

void fclient(struct client_server_comunication *csc);

int main(){
    struct client_server_comunication csc = {12345,678,90};
    fclient(&csc);
}


void fclient(struct client_server_comunication *csc){
    int clientfd,r,opt=1;
    struct sockaddr_in client;
    socklen_t socklen;
    char buffer[1024] = { 0 };
    //char buffer[10];

    char* hello = "Este mensaje salió desde client";

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd<0){
        perror("Error en socket()\n");
        exit(0);
    }
    //setsockopt(clientfd,SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));

    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    //r = inet_aton("127.0.0.1",&client.sin_addr);
    r = inet_pton(AF_INET,"127.0.0.1",&client.sin_addr);
    if (r<=0){
        printf("Error en inet_aton()");
    }
    //NO SE HIZO EL bzero() NO SÉ POR QUÉ


    //r = connect(clientfd,(struct sockaddr *)&client,(socklen_t)sizeof(struct sockaddr));
    r = connect(clientfd,(struct sockaddr *)&client,sizeof(client));
    if(r<0){
        perror("Error en connect\n");
        exit(0);
    }



    //To send message
    //struct client_server_comunication csc = {123,987,465};

    //send(clientfd,hello,strlen(hello),0);
    send(clientfd,csc,sizeof(struct client_server_comunication),0);

    r = recv(clientfd, buffer,10,0);
    if(r<0){
        perror("Error en recv\n");
        exit(0);
    }
    printf("El valor de r es: %d\n",r);
    buffer[r]=0;
    //for(int i = 0;i<(sizeof(buffer)/sizeof(char));i++){
    //    //printf("%d\n",i);
    //    printf("%c\n",buffer[i]);
    //}
    printf("El mensaje es: \n %s\n",buffer);
    close(clientfd);
}
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"../header.h"




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



void creating_client(struct sharerow *sr1);

int main(){
	/*
	Script para la interface
	*/

    FILE *fshare;
    fshare = fopen("share.bin","w+b");
    if(fshare==NULL){
        printf("Ocurrió un problema con el opening de share.bin!\n");
    }
    struct sharerow sr1={0,0,0};

    short int opcion;

    
    while(1) {
        system("clear");
 		printf("\nBienvenido\n\n"
 			"1.  Ingresar origen\n"
 			"2.  Ingresar destino\n"
 			"3.  Ingresar hora\n"
 			"4.  Buscar tiempo de viaje medio\n"
 			"5.  Salir\n"
 			"*********************************************\n¿Elige la opción?: ");
	 			
	 	scanf("%hd", &opcion);

	 	switch(opcion) {
	 		case 1:
	  			system("clear");
	  			printf("\nIngrese ID del origen: ");
	  			scanf("%hd", &sr1.sourceid);
                if (sr1.sourceid<0 || sr1.sourceid>1160){
					system("clear");
                    printf("\nValor incorrecto, se espera un número entre 1 y 1160\n");
                    sleep(3);
                    break;    
                }
				break;

	     	case 2:
	 			system("clear");
	  			printf("\nIngrese ID del destino: ");
	  			scanf("%hd", &sr1.dstid);
                if (sr1.dstid<0 || sr1.dstid>1160){
                    system("clear");
                    printf("\nValor incorrecto, se espera un número entre 1 y 1160\n");
                    sleep(3);  
                    break;      
                }
				break;

	     	case 3:
	  			system("clear");
	  			printf("\nIngrese hora del día: ");
	  			scanf("%hd",&sr1.hod);
                if (sr1.hod<0 || sr1.hod>23){
                    system("clear");
                    printf("\nValor incorrecto, se espera un número entre 0 y 23\n");
                    sleep(3);
                    break;        
                }

	  			break;
	     	case 4:
				system("clear");
                //fwrite(&sr1,sizeof(struct sharerow),1,fshare);
                //fclose(fshare);
                //system("gcc find.c -o find");
                //char *args[]={"./find",NULL};
                //execv(args[0],args); //No aparentado
				//struct client_server_comunication csc = {12345,678,90};
				//Aquí debería existir el for y sale!
    			creating_client(&sr1);

	     	case 5:
				//system("clear");
				//fclose(fshare);
				printf("\nHasta Luego!\n\n");
				exit(0);
				//return(1);	
	     	default:
				system("clear");
                printf("\n¡Esa opción no es válida!\n");
                sleep(1);
                break;
	 	}
    }
    return 0;
}


void creating_client(struct sharerow *sr1){
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
    //struct sharerow sr1 = {123,987,465};

    //send(clientfd,hello,strlen(hello),0);
    send(clientfd,sr1,sizeof(struct sharerow),0);
    struct row r1;
    r = recv(clientfd, &r1,sizeof(struct row),0);
    if(r<0){
        perror("Error en recv\n");
        exit(0);
    }
    
    printf("\nTiempo de viaje medio: %0.2f\n\n",r1.mean);
    printf("mpos: %d \nnpos: %d\n",r1.mpos,r1.npos);
    printf("sourceid: %d \ndstid: %d \nhod: %d\n",r1.sourceid,r1.dstid,r1.hod);

    //printf("a: %d, b: %d, c: %d\n",r1.sourceid,r1.dstid,r1.hod);

    //printf("El valor de r es: %d\n",r);
    //buffer[r]=0;
    //for(int i = 0;i<(sizeof(buffer)/sizeof(char));i++){
    //    //printf("%d\n",i);
    //    printf("%c\n",buffer[i]);
    //}
    //printf("El mensaje es: \n %s\n",buffer);

    close(clientfd);
}
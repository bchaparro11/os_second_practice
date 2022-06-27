#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> //Para ejecutar close()
#include"../header.h" //Para usar los structs específicos par el funcionamiento
#include <sys/types.h> //Para usar socket() junto con la siguiente
#include <sys/socket.h>
#include <netinet/in.h> //Para usar sockaddr_in
#include <arpa/inet.h> //Para usar inet_ntop
//#include <netdb.h>
//#include <strings.h>

#define PORT 3535 //Puerto por defecto para la conexión entre client y server
#define SERVER_IP "127.0.0.1" //Dirección de LOOPBACK para conectar con el server

void creating_client(struct sharerow *sr1); //Declarando función para crear un cliente

int main(){
    /*
    - Struct utilizado para almacenar la información del menu
    - La definición del struct está en ../header.h
    */
    struct sharerow sr1={0,0,0}; 

    short int opcion; //Para poder ejecutar el switch

    while(1) {
        system("clear"); //Limpiando la consola para que se vea mejor el menú
 		printf("\nBienvenido\n\n"
 			"1.  Ingresar origen\n"
 			"2.  Ingresar destino\n"
 			"3.  Ingresar hora\n"
 			"4.  Buscar tiempo de viaje medio\n"
 			"5.  Salir\n"
 			"*********************************************\n¿Elige la opción?: "); //Imprimiendo el menú
	 			
	 	scanf("%hd", &opcion); //Recibiendo la opción del usuario

	 	switch(opcion) {
	 		case 1:
	  			system("clear"); //Limpiando la consola para que se vea mejor la interacción en case 1
	  			printf("\nIngrese ID del origen: ");
	  			scanf("%hd", &sr1.sourceid); //Almacenando el valor ingresado por el usuario en struct sharerow sr1
                if (sr1.sourceid<0 || sr1.sourceid>1160){
					system("clear");
                    printf("\nValor incorrecto, se espera un número entre 1 y 1160\n"); //Manejando valores incorrectos
                    sleep(3);
                    break;    
                }
				break;

	     	case 2:
	 			system("clear"); //Limpiando la consola para que se vea mejor la interacción en case 1
	  			printf("\nIngrese ID del destino: ");
	  			scanf("%hd", &sr1.dstid); //Almacenando el valor ingresado por el usuario en struct sharerow sr1
                if (sr1.dstid<0 || sr1.dstid>1160){
                    system("clear");
                    printf("\nValor incorrecto, se espera un número entre 1 y 1160\n"); //Manejando valores incorrectos
                    sleep(3);  
                    break;      
                }
				break;

	     	case 3:
	  			system("clear"); //Limpiando la consola para que se vea mejor la interacción en case 1
	  			printf("\nIngrese hora del día: "); 
	  			scanf("%hd",&sr1.hod); //Almacenando el valor ingresado por el usuario en struct sharerow sr1
                if (sr1.hod<0 || sr1.hod>23){
                    system("clear");
                    printf("\nValor incorrecto, se espera un número entre 0 y 23\n"); //Manejando valores incorrectos
                    sleep(3);
                    break;        
                }

	  			break;
	     	case 4:
				system("clear"); //Limpiando la consola para que se vea mejor la interacción en case 1
    			creating_client(&sr1); //Creando el cliente. Mirar función para entenderlo
                //No se pone aposta el break; para que entre al case 5 también
	     	case 5:
				printf("\nHasta Luego!\n\n");
				exit(0);
	     	default:
				system("clear"); //Limpiando la consola para que se vea mejor la interacción en case 1
                printf("\n¡Esa opción no es válida!\n"); //Manejando valores incorrectos
                sleep(1);
                break;
	 	}
    }
    return 0;
}

void creating_client(struct sharerow *sr1){
    int clientfd,r; //clientfd es el descriptor del cliente y r es usado para manejar errores
    struct sockaddr_in client; //Struct para poder ingresar datos de server y conectarse a él

    clientfd = socket(AF_INET, SOCK_STREAM, 0); //Definiendo el cliente
    if(clientfd<0){
        perror("Error en socket()\n"); //Manejando error de socket()
        exit(0);
    }

    client.sin_family = AF_INET; //Para decir que se usará IPV4
    client.sin_port = htons(PORT); //Usando el puerto 3535 y manejando el endinismo

    r = inet_pton(AF_INET,SERVER_IP,&client.sin_addr); //Para guardar en client.sin_addr la ip del server en binario
    if (r<=0){
        printf("Error en inet_aton()");//Manejando error de inet_pton()
    }

    r = connect(clientfd,(struct sockaddr *)&client,sizeof(client)); //Conectándose al servidor
    if(r<0){
        perror("Error en connect\n"); //Manejando error de connect()
        exit(0);
    }

    send(clientfd,sr1,sizeof(struct sharerow),0); //Enviando el struct sharerow obtenido en el menú

    struct row r1; //Struct row, que está en ../header.h para recibir toda la información del viaje gestionada por el server

    r = recv(clientfd, &r1,sizeof(struct row),0); //Para recibir toda la información del viaje gestionada por el server
    if(r<0){
        perror("Error en recv\n"); //Manejando error de recv()
        exit(0);
    }
    
    //Las siguientes tres líneas de código son para imprimir la información obtenida en recv()
    printf("\nTiempo de viaje medio: %0.2f\n\n",r1.mean);
    printf("mpos: %d \nnpos: %d\n",r1.mpos,r1.npos);
    printf("sourceid: %d \ndstid: %d \nhod: %d\n",r1.sourceid,r1.dstid,r1.hod);

    close(clientfd); //Cerrando el client declarado al principio de esta función
}
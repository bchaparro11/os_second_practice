#include<stdio.h>
#include<stdlib.h>
#include"../header.h"

/*
Esta función es para leer un struct row dependiendo de la posición del puntero
FILE que se pasa como argumento, después de leer el puntero se mueve, entonces
esta función también devuelve el puntero a la posición inicial que se pasó en
el argumento
*/
void reversetravel(FILE *filedata, struct row *r1);

int main(){
    /*
    Este script es para dejar el archivo binario data.bin con los valores correspondientes
    para el miembro npos de cada struct row. Esto permite generar la pseudo lista enlazada
    que el profesor asignó. Se empieza desde el último registro o struct row almacenado
    hasta el primero
    - Este script dura menos de 40 seg en ejecutarse
    */

    FILE *filehash;
    filehash = fopen("../toQuery/hashtable.bin","r+b");
    if(filehash==NULL){
        printf("Ocurrió un problema con el opening del FILE hashtable.bin");
        exit(0);
    }

    FILE *filedata;
    filedata = fopen("../toQuery/data.bin","r+b");
    if(filedata==NULL){
        printf("Ocurrió un problema con el opening del FILE data.bin");
        exit(0);
    }
    
    
    struct row r1;
    struct hashrow hr1;

    //Lo que primero se hace es sacar la útlima posición del data.bin
    fseek(filedata,0,SEEK_END);
    
    while(1){

        /*
        Primera sección:
        Para obtener los structs correspondientes de hashrow en hashtable.bin
        y row de data.bin
        */
        if(ftell(filedata)==0){
    
            printf("\nYa se hizo la pseudo linked list en data.bin!\n");
            break;
        }
        
        reversetravel(filedata,&r1);

        //El valor de la mitad es para hallar la posición correcta en hashtable.bin
        fseek(filehash,(r1.sourceid-1)*sizeof(struct hashrow),SEEK_SET);

        fread(&hr1,sizeof(struct hashrow),1,filehash);

        fseek(filehash,-sizeof(struct hashrow),SEEK_CUR);

        /*
        Segunda sección:
        Desde esta línea de código hasta el final lo que se hace es asignar los valores
        correspondientes para el miembro npos de cada struct row y para el miembro img
        de cada struct hashrow (el que guarda la primera iteración del sourceid en data.bin)
        */
        if(hr1.img!=0){
            //Si llega acá es porque hay una colisión entonces
            r1.npos=hr1.img;
            fwrite(&r1,sizeof(struct row),1,filedata);
            fseek(filedata,-sizeof(struct row),SEEK_CUR);

        }
        /*
        Si llega la primera vez (no hay colisión pq es la 1ra vez) se debe escribir la posición,
        Si llega la segunda vez (hay colisión) también se debe escribir la nueva posición
        Por esa razón se escribe la siguiente ínea de código
        */

        hr1.img = r1.mpos;

        fwrite(&hr1,sizeof(struct hashrow),1,filehash);

        fseek(filehash,-sizeof(struct hashrow),SEEK_CUR);

    }

    fclose(filehash);
    fclose(filedata);

    return 0;
}




void reversetravel(FILE *filedata, struct row *r1){
    fseek(filedata,-sizeof(struct row),SEEK_CUR);

    fread(r1,sizeof(struct row),1,filedata);

    fseek(filedata,-sizeof(struct row),SEEK_CUR);
}
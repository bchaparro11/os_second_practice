#include<stdio.h>
#include<stdlib.h>
#include"../header.h" //Para obtener los structs necesarios para ejecutar el código de este script

int main(){
    /*
    Puntero utilizado para manipular el archivo hashtable.bin utilizado para implementar
    la pseudo hash table
    */
    FILE *freserve;
    freserve = fopen("../toQuery/hashtable.bin","w+b");
    if(freserve==NULL){
        printf("Ocurrió un error con el opening del file hashtable.bin");
    }

    struct hashrow hr1; //Se declara el struct hashrow para poder guardar en binario en hashtable.bin
    hr1.img = 0; //Se inicializa el atributo .img en cero para que luego sea llenado con valores correspondientes en el script 3.c
    
    //Ciclo que crea toda la estructura del archivo hashtable.bin
    for(int i=0;i<1300;i++){
        fwrite(&hr1,sizeof(struct hashrow),1,freserve);
    }
    
    printf("Terminó la reserva de memoria en hashtable.bin!\n");
    fclose(freserve);
    return 0;
}
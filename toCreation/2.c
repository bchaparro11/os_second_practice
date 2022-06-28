#include<stdio.h>
#include<stdlib.h>
#include"../header.h"

int main(){
    /*
    Este script es para crear un archivo llamado hashtable.bin que guardará
    struct hashrow, que está en ../header.h, con un valor dado por default
    que es cero. Este archivo guardará la posición del primer struct row con
    un sourceid específico. La posición que guarda es la específica del archivo
    binario que se llama data.bin
    - Este script se ejecuta al instante
    */
    FILE *freserve;
    freserve = fopen("../toQuery/hashtable.bin","w+b");
    if(freserve==NULL){
        printf("Ocurrió un error con el opening del file hashtable.bin");
    }

    //Se inicializa el struct hashrow para poder guardar en binario en hashtable.bin
    struct hashrow hr1;
    hr1.img = 0;
    
    for(int i=0;i<1300;i++){
        fwrite(&hr1,sizeof(struct hashrow),1,freserve);
        //printf("Byte terminal: %ld\n",ftell(freserve));
    }
    
    printf("Terminó la reserva de memoria en hashtable.bin!\n");
    fclose(freserve);
    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include"../header.h" //Para obtener los structs necesarios para ejecutar el código de este script

int main(){
    /*
    El siguiente puntero es para sacar todos los datos del .csv llamado csvfinal.csv
    */
    FILE *ftake;
    ftake = fopen("csvfinal.csv","r");
    if(ftake==NULL){
        printf("Hubo un error con el opening del csv.csv\n");
        exit(0);
    }

    /*
    El siguiente puntero se utiliza para crear el archivo data.bin que se utilizará para
    implementar las pseudo linked-lists necesarias para implementar la pseudo hash table
    que se implementa con hashtable.bin
    */
    FILE *fsave;
    fsave = fopen("../toQuery/data.bin","w+b");
    if(fsave==NULL){
        printf("Hubo un error con el opening del all.bin\n");
        exit(0);
    }
    
    struct row r1; //Struct utilizado para guardar información en data.bin
    r1.npos = 0; //Inicializando el atributo NextPosition para ir implementando la pseudo linked-lists

    /*
    Con este ciclo se pasa por todos los registros de csvfinal.csv y se guarda cada uno en
    data.bin. También se asigna una posición a cada struct row y esto se hace mediante el 
    miembro del struct mp (abreviación para MyPosition)
    */
    for(int i=1;1;i++){       
        int end = fscanf(ftake,"%hd,%hd,%hd,%f,%*s",&r1.sourceid,&r1.dstid,&r1.hod,&r1.mean); //Leyendo registros de csvfinal.csv
        r1.mpos=i; //Inicializando la posición de cada registro para guardarla posteriormente en data.bin
        if(end==EOF){
            printf("Terminó lectura de csvfinal.csv y se guardó de manera binario en data.bin!\n");
            break;
        }

        fwrite(&r1,sizeof(struct row),1,fsave); //Guardando en data.bin todos los datos obtenidos en r1
    }
    fclose(ftake); //Cerrando puntero que gestiona el archivo csvfinal.csv
    fclose(fsave); //Cerrando puntero que gestiona el archivo data.bin
    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include"../header.h"

int main(){
    /*
    Al .csv original se le borró el título y un salto de línea adicional que tenía
    después del último registro.
    - Este script dura menos de 10 seg en ejecutarse
    */

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
    El siguiente puntero se va a utilizar para guardar SÓLO LOS PRIMEROS CUATRO DATOS DE
    cada row sacado del .csv, ESTO PERMITE OPTIMIZAR LA MEMORIA EN DISCO, en un archivo 
    binario llamado data.bin. Los datos de cada row se almacenan en un struct y se 
    guardan de manera binaria luego en data.bin. TODOS
    LOS STRUCTS UTILIZADOS EN TODOS LOS .c, SE USAN CON LOS TIPOS DE DATOS
    MÁS PRECISOS COMO short int PARA OPTIMIZAR LA MEMORIA EN DISCO. VER LOS STRUCTS EN
    header.h 
    */
    FILE *fsave;
    fsave = fopen("../toQuery/data.bin","w+b");
    if(fsave==NULL){
        printf("Hubo un error con el opening del all.bin\n");
        exit(0);
    }
    
    /*Declarando el struct para poder guardar en binario en data.bin y dandole un valor por defecto
    a npos que será 0. Este valor luego va a ser cambiado en el script 3.c para 
    que coja sus valores correspondientes
    */
    struct row r1;
    r1.npos = 0;

    /*
    Con este ciclo se pasa por todos los registros de csvfinal.csv y se guarda cada uno en
    data.bin. También se asigna una posición a cada struct row y esto se hace mediante el 
    miembro del struct mp
    */
    for(int i=1;1;i++){
        //El %hd es para que reconozca el short int        
        int end = fscanf(ftake,"%hd,%hd,%hd,%f,%*s",&r1.sourceid,&r1.dstid,&r1.hod,&r1.mean);

        r1.mpos=i;
        if(end==EOF){
            printf("Terminó lectura de csvfinal.csv y se guardó de manera binario en data.bin!\n");
            break;
        }

        fwrite(&r1,sizeof(struct row),1,fsave);
    }
    fclose(ftake);
    fclose(fsave);

    return 0;
}
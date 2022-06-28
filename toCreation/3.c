#include<stdio.h>
#include<stdlib.h>
#include"../header.h" //Para obtener los structs necesarios para ejecutar el código de este script

void reversetravel(FILE *filedata, struct row *r1); //Función para recorrer el archivo data.bin desde el final hasta el principio

int main(){
    //Puntero utilizado para gestionar el archivo hashtable.bin utilizado para implementar la pseduo hash table
    FILE *filehash;
    filehash = fopen("../toQuery/hashtable.bin","r+b");
    if(filehash==NULL){
        printf("Ocurrió un problema con el opening del FILE hashtable.bin"); //Manejando errores de fopen()
        exit(0);
    }

    //Puntero utilizado para gestionar el archivo data.bin utilizado para implementar las pseudo linked-lists que necesita la pseudo hash table
    FILE *filedata;
    filedata = fopen("../toQuery/data.bin","r+b");
    if(filedata==NULL){
        printf("Ocurrió un problema con el opening del FILE data.bin"); //Manejando errores de fopen()
        exit(0);
    }
    
    
    struct row r1; //Stuct usado para leer y guardar información en data.bin
    struct hashrow hr1; //Stuct usado para leer y guardar información en hashtable.bin

    //Lo que primero se hace es sacar la útlima posición del data.bin
    fseek(filedata,0,SEEK_END);
    
    while(1){
        if(ftell(filedata)==0){//Para saber si ya llegó al principio de data.bin
            printf("\nYa se hizo la pseudo linked list en data.bin!\n"); //Manejando errores de ftell()
            break;
        }
        
        reversetravel(filedata,&r1); //Para leer un registro específico en el archivo data.bin

        fseek(filehash,(r1.sourceid-1)*sizeof(struct hashrow),SEEK_SET); //Posicionando el puntero en el lugar correcto de hashtable.bin para que queda con la posición del primer registro de un valor específico de origen
        fread(&hr1,sizeof(struct hashrow),1,filehash); //Leyendo el valor del registro
        fseek(filehash,-sizeof(struct hashrow),SEEK_CUR); //Posicionando el puntero en el lugar antes de la lectura

        /*
        Desde esta línea de código hasta el final lo que se hace es asignar los valores
        correspondientes para el miembro npos de cada struct row y para el miembro img
        de cada struct hashrow (el que guarda la primera iteración del sourceid en data.bin)
        */
        if(hr1.img!=0){
            //Si llega acá es porque hay una colisión entonces
            r1.npos=hr1.img; //Se actualiza el valor que implementa la pseudo linked-list
            fwrite(&r1,sizeof(struct row),1,filedata); //Se guarda todo el struct r1 en data.bin
            fseek(filedata,-sizeof(struct row),SEEK_CUR); //Se vuelve a la posición del puntero antes de la lectura anterior

        }
        /*
        Las siguientes tres líneas de código siempre deben ejecutarse y son para actualizar los
        valores del archivo hashtable.bin, que implementa la pseudo hash table, cada vez que se
        lee un registro en data.bin
        */
        hr1.img = r1.mpos; //Actualiza el valor en la estructura hr1 con la posición del último registro encontrado con un valor de sourceid específico leyendo de abajo hacía arriba data.bin
        fwrite(&hr1,sizeof(struct hashrow),1,filehash); //Guardando el valor del struct hr1 en hashtable.bin
        fseek(filehash,-sizeof(struct hashrow),SEEK_CUR); //Vuelve el puntero a la posición existente antes de la lectura

    }

    fclose(filehash); //Cerrando el FILE usado para gestionar hashtable.bin
    fclose(filedata); //Cerrando el FILE usado para gestionar data.bin
    return 0;
}




void reversetravel(FILE *filedata, struct row *r1){
    fseek(filedata,-sizeof(struct row),SEEK_CUR); //Se devuelve los bytes correspondientes para que inicie la lectura desde el princio de un struct específico
    fread(r1,sizeof(struct row),1,filedata); //Lee los datos del registro específico
    fseek(filedata,-sizeof(struct row),SEEK_CUR); //Vuelve a dejarlo en la posición antes de la lectura
}
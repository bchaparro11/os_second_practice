/*
Todos los structs están con el padding correspondiente
*/
struct row{
    /*
    Esta estructura se usa para:
    - Llenar el archivo data.bin con los datos necesarios y correspondientes a cada registro de csvfinal.csv
    - Usarlo en la función searching() ubicada en el script p2-server.c
    */
    int mpos,npos; // mpos = MyPosition guarda la posición de un registro en data.bin; npos = NextPosition es para efectuar la lista enlazada
    short int sourceid,dstid,hod,isfound; //sourceid, dstid y hod es para hacer la busqueda en data.bin y isfound es para saber si se encontró alguna coincidencia
    float mean; //Es el valor medio de viaje se encuentra siempre y cuando se haya encontrado una coincidencia en data.bin cuando se ejecuta searching()
};

struct hashrow{
    /*
    Para crear y llenar de valores hashtable.bin, archivo para efectuar la pseudo hashtable
    */
    int img; //Almacena la posicion en data.bin del primer registro con un valor de origen específico
};

struct sharerow{
    /*
    Sirve para almacenar los datos que se obtienen desde el menú para luego transmitirlos al server desde el client
    */
    short int sourceid,dstid,hod; //sourceid: almacena el origen, dstid: almacena el destino, hod: almacena la hora del día
};
    /*
    Usando los tipos de dato más precisos con el padding más correcto
    para optimizar la memoria en disco. Source para padding:
    https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/
    */
struct row{
    /*
    mpost = My position
    npost = Next position
    */
    int mpos,npos;
    short int sourceid,dstid,hod,isfound;
    float mean;
};

struct hashrow{
    /*
    Este no puede ser un short int porque este guarda las posiciones
    y las posiciones llegan hasta casi seis millones
    que es un número que no puede guardar un short int
    */
    int img;
};

struct sharerow{
    short int sourceid,dstid,hod;
};
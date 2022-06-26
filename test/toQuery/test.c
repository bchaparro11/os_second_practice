#include <stdio.h>
#include <stdlib.h>

struct row{
    int a,b;
};

int main(){
    struct row r1={1,2};
    printf("a: %d, b: %d\n",r1.a,r1.b);  
    
}
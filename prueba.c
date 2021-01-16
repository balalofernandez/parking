#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//funcion por valor
void valor(int **valor){
    printf("bueno\n");
    printf("%i\n", **valor);
    // *valor = 4;
}

int main(){
    /*int *a;
    a = (int *) malloc(sizeof(int)*2);
    a[0] = 1;
    a[1] = 2;
    int b;
    valor(&a[0]);
    printf("%i\n",a[0]);*/
    sleep(3);
    printf(".");
}
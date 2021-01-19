#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct vehiculo{
    int numero;
    int camion;//booleano que verifica si es camión o no
} TVehiculo;
typedef struct cola{
    TVehiculo *vehiculo;
    struct cola *sig;
}TCola;

TCola *cabezaTotal=NULL, *finTotal=NULL;
TCola *cabezaCamiones=NULL, *finCamiones=NULL;
TCola *aux;

void insertarTotal(TVehiculo *vehiculo){
    aux=(TCola *)malloc(sizeof(TCola));
    aux->vehiculo = vehiculo;
    aux->sig = NULL;
    if (finTotal==NULL)
        finTotal=cabezaTotal=aux;
    else
    {
        finTotal->sig = aux;
        finTotal = aux;
    }
}

void extraerTotal(){
    if(cabezaTotal){
        aux = cabezaTotal;
        cabezaTotal = cabezaTotal->sig;
        free(aux);
    }
}

int main(){
    /*int *a;
    a = (int *) malloc(sizeof(int)*2);
    a[0] = 1;
    a[1] = 2;
    int b;
    valor(&a[0]);
    printf("%i\n",a[0]);*/
    TVehiculo *prueba;
    prueba = (TVehiculo *) malloc(sizeof(TVehiculo));
    TVehiculo vehiculo1;
    vehiculo1.numero = 1;
    vehiculo1.camion = 1;
    TVehiculo vehiculo2;
    vehiculo2.numero = 2;
    vehiculo2.camion = 2;
    TVehiculo vehiculo3;
    vehiculo3.numero = 3;
    vehiculo3.camion = 3;
    TVehiculo *aux;
    vehiculo2.numero = 0;
    vehiculo2.camion = 0;

    insertarTotal(&vehiculo1);
    insertarTotal(&vehiculo2);
    insertarTotal(&vehiculo3);
    printf("%i\n",cabezaTotal->sig->sig->vehiculo->numero);
    extraerTotal();
    extraerTotal();
    printf("%i\n",cabezaTotal->vehiculo->numero);
}
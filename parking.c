#include <pthread.h>
#include <stdio.h>

void *coche();
int buscaLibre(int **parking, int plantas, int plazas, int *plantaLibre, int *plazaLibre); //funcion que busca la plaza libre

int main(int argc, char* argv[]){
    int plazas;
    int plantas;
    int coches;
    int camiones;
    int i;
    int **parking; //parking [planta][plaza]
    pthread_t *tids; //lo utilizo para guardar todos los tids
    printf("%i\n", argc);
    switch (argc)
    {
    case 1:
        printf("Error.No se han indicado las PLAZAS DEL PARKING\n");
        return 0;
        break;
    case 2:
        printf("No se han introducido las plantas del parking, se inicializarán a 1\n");
        sscanf(argv[1], "%i", &plazas);
        printf("Numero de plazas %i\n", plazas);
        plantas = 1;
        coches = 2*plazas*plantas;
        camiones = 0;
        break;
    case 3:
        // plazas = (int) argv[1];
        sscanf(argv[1], "%i", &plazas);
        // plantas = (int) argv[2];
        sscanf(argv[2], "%i", &plantas);
        coches = 2*plazas*plantas;
        camiones = 0;
        break;
    case 4:
        // plazas = (int) argv[1];
        sscanf(argv[1], "%i", &plazas);
        // plantas = (int) argv[2];
        sscanf(argv[2], "%i", &plantas);
        // coches = (int) argv[3];
        sscanf(argv[3], "%i", &coches);
        camiones = 0;
        break;
    case 5:
        // plazas = (int) argv[1];
        sscanf(argv[1], "%i", &plazas);
        // plantas = (int) argv[2];
        sscanf(argv[2], "%i", &plantas);
        // coches = (int) argv[3];
        sscanf(argv[3], "%i", &coches);
        // coches = (int) argv[4];
        sscanf(argv[4], "%i", &camiones);
        break;
    default:
        printf("hola");
        return(1);
        break;
    }

    //creamos tantos threads como coches tengo
    for (i=0;i<coches;i++){
        tids[i];
        pthread_create(&tids,NULL,coche,NULL);
    }
}

void *coche();
//funcion que devuelve true o false si hay hueco para camion y la posicion de la plaza libre
int buscaLibre(int **parking, int plantas, int plazas, int *plantaLibre, int *plazaLibre){
    int i=0;
    int j;
    int libre=0;
    while (!libre && i<plantas){
        j=0;
        while (!libre && j<plazas){
            if ((parking[i][j]==0) && j< plazas-1){
                libre=(parking[i][j+1]==0);
            }
            j++;
        }
        i++;
    }
    *plantaLibre = --i;
    *plazaLibre = --j;
    if (!libre){
        *plantaLibre = -1;
        *plazaLibre = -1;
    }
    return(libre);
}

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//VARIABLES GLOBALES 
int **parking; //parking [planta][plaza]
int plazasLibres;

pthread_mutex_t mutex;

//Tenemos la condición de que el parking está lleno
pthread_cond_t no_lleno;

void *coche();
int buscaLibre(int **parking, int plantas, int plazas, int *plantaLibre, int *plazaLibre); //funcion que busca la plaza libre
void *aparcar(int *planta, int *plaza, int numCoche);
void *desaparcar(int planta,int plaza);
int main(int argc, char* argv[]){
    int plazas;
    int plantas;
    int coches;
    int camiones;
    int i;
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
        plazasLibres = plazas*plantas;
        camiones = 0;
        break;
    case 3:
        // plazas = (int) argv[1];
        sscanf(argv[1], "%i", &plazas);
        // plantas = (int) argv[2];
        sscanf(argv[2], "%i", &plantas);
        coches = 2*plazas*plantas;
        plazasLibres = plazas*plantas;
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
        plazasLibres = plazas*plantas;
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
        plazasLibres = plazas*plantas;
        break;
    default:
        printf("hola");
        return(1);
        break;
    }
    pthread_mutex_init(&mutex, NULL); //Iniciamos el mutex
    pthread_cond_init(&no_lleno, NULL);

    //creamos tantos threads como coches tengo
    tids = (pthread_t *) malloc(sizeof(pthread_t));
    for (i=0;i<coches;i++){
        tids[i];
        pthread_create(&tids[i],NULL,coche,(void *) (i+1));
    }
}

void *coche(void* nCoche){
    int plantaAux,plazaAux;
    int numCoche = *(int *) nCoche;
    //Hacemos algo para que espere a entrar
    int espera = (rand()% 8) +1;
    sleep(espera);
    pthread_mutex_lock(&mutex);
    while (plazasLibres != 0)
    {
        pthread_cond_wait(&no_lleno, &mutex);
    }
    //Seccion crítica
    plazasLibres--;
    aparcar(&plantaAux,&plazaAux);//esta función busca un aparcamiento y devuelve la posición
    pthread_mutex_unlock(&mutex);
    //el vehiculo ha sido estacionado
    espera = (rand()% 8) +1;
    sleep(espera);
    pthread_mutex_lock(&mutex);
    plazasLibres++;
    desaparcar(plantaAux,plazaAux);
    pthread_mutex_unlock(&mutex);
};

void *aparcar(int *plantaAux, int *plazaAux, int numCoche){
    int i;
    int j;
    for (i=0, i<plantas, )
}
void *desaparcar(int planta,int plaza){

}


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


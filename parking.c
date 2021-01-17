#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//VARIABLES GLOBALES 
int **parking; //parking [planta][plaza]
int plazasLibres;
int plazas;
int plantas;
int coches;
int camiones;

pthread_mutex_t mutex;

//Tenemos la condición de que el parking está lleno
pthread_cond_t no_lleno;
pthread_cond_t huecoCamion;

void *coche();
void *camion();
void aparcarCamion(int plantaAux, int plazaAux, int numCamion);
int buscaLibre(int *plantaLibre, int *plazaLibre); //funcion que busca la plaza libre

int main(int argc, char* argv[]){

    int i;
    pthread_t *tidsCoche; //Guarda tid del coche
    pthread_t *tidsCamion; //Guarda tid del camión
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

    plazasLibres = plazas*plantas;//Inicializamos las plazas libres en función de las plazas y plantas del parking
    //Reservamos espacio para el parking [planta][plaza]
    parking = (int **) calloc(plantas,sizeof(int *));

    for(int i = 0; i < 2; i++)
        parking[i] = (int *) calloc(plazas,sizeof(int));

    //Iniciamos el mutex y las condiciones
    pthread_mutex_init(&mutex, NULL); 
    pthread_cond_init(&no_lleno, NULL);
    pthread_cond_init(&huecoCamion, NULL);
    //creamos tantos threads como coches tenemos
    tidsCoche = (pthread_t *) malloc(sizeof(pthread_t)*coches);
    for (i=0;i<coches;i++){
        tidsCoche[i];
        pthread_create(&tidsCoche[i],NULL,coche,NULL);
    }
    //Creamos tantos threads como camiones tenemos
    tidsCamion = (pthread_t *) malloc(sizeof(pthread_t)*camiones);
    for (i=0;i<coches;i++){
        tidsCoche[i];
        pthread_create(&tidsCoche[i],NULL,coche,(void *) (i+1));
    }
}

void *coche(){
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
    aparcar();//la esta función busca un aparcamiento y devuelve la posición
    
};

//Funcion que devuelve true o false si hay hueco para camion y la posicion de la plaza libre
int buscaLibre(int *plantaLibre, int *plazaLibre){
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

void *camion(void *nCamion){
    int plantaLibreAux,plazaLibreAux;//Para que obtener el sitio que está libre en el parking
    int numCamion = *(int *)nCamion; //Numero del camión 
    //Hacemos random para que espere a entrar
    int espera = (rand()% 8) +1;
    sleep(espera);
    pthread_mutex_lock(&mutex);
    while (!buscaLibre(&plantaLibreAux, &plazaLibreAux))
    {
        pthread_cond_wait(&huecoCamion, &mutex);
    }
    //Seccion crítica
    plazasLibres -= 2;
    aparcarCamion(plantaLibreAux,plazaLibreAux, numCamion);//la esta función busca un aparcamiento y devuelve la posición
    pthread_mutex_unlock(&mutex);//Liberamos el mutex para que otros puedan acceder al parking
    espera = (rand()% 8) +1;
    sleep(espera);//Esperamos un tiempo aleatorio
    pthread_mutex_lock(&mutex);//peleamos por el mutex 
    plazasLibres +=2;
    desaparcarCamion(plantaLibreAux, plazaLibreAux);
    pthread_cond_signal(&no_lleno);
    pthread_cond_signal(&huecoCamion);
    pthread_mutex_unlock(&mutex);
};

void aparcarCamion(int plantaAux, int plazaAux, int numCamion){

}


void desaparcarCamion(int plantaAux, int plazaAux){

}

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
void aparcar(int *planta, int *plaza, int numCoche);
void desaparcar(int planta,int plaza);
void *camion();
void aparcarCamion(int plantaAux, int plazaAux, int numCamion);
void desaparcarCamion(int plantaAux, int plazaAux);
int buscaLibre(int *plantaLibre, int *plazaLibre); //funcion que busca la plaza libre
void muestraParking();

int main(int argc, char* argv[]){
    int i;
    int *nCoches;
    int *nCamiones;
    pthread_t tid;
    printf("Se han introducido %i argumentos\n", argc);
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
        printf("Numero de plantas %i\n", plantas);
        coches = 2*(plazas*plantas);
        printf("Numero de coches %i\n", coches);
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
        
        return(1);
        break;
    }
    plazasLibres = plazas*plantas;//Inicializamos las plazas libres en función de las plazas y plantas del parking
    //Reservamos espacio para el parking [planta][plaza]
    parking = (int **) calloc(plantas,sizeof(int *));
    for(i = 0; i < plantas; i++)
        parking[i] = (int *) calloc(plazas,sizeof(int));
    //Iniciamos el mutex y las condiciones
    pthread_mutex_init(&mutex, NULL); 
    pthread_cond_init(&no_lleno, NULL);
    pthread_cond_init(&huecoCamion, NULL);    
    //creamos tantos threads como coches tenemos
    fprintf(stderr,"coches %i\n",coches);
    fprintf(stderr,"camion %i\n",camiones);
    nCoches = (int *) calloc(coches, sizeof(int));
    for (i=0;i<coches;i++){
        nCoches[i] = (i+1);
        pthread_create(&tid,NULL,coche,(void *) &nCoches[i]);
    }
    //Creamos tantos threads como camiones tenemos
    nCamiones = (int *) calloc(camiones, sizeof(int));
    for (i=0;i<camiones;i++){
        nCamiones[i] = (i+1);
        pthread_create(&tid,NULL,camion,(void *) &nCamiones[i]);
    }
    while(1);
    return(1);
}

void *coche(void* nCoche){
    int numCoche = *(int *) nCoche;
    int plantaAux,plazaAux;
    int espera;
    while (1){
        //Hacemos algo para que espere a entrar
        espera = (rand()% 25) +1;
        sleep(espera);
        pthread_mutex_lock(&mutex);
        while (!plazasLibres)
        {
            pthread_cond_wait(&no_lleno, &mutex);
        }
        fprintf(stderr,"Soy el coche %i\n", numCoche);
        //Seccion crítica
        plazasLibres--;
        aparcar(&plantaAux,&plazaAux, numCoche);//esta función busca un aparcamiento y devuelve la posición
        muestraParking();
        pthread_mutex_unlock(&mutex);
        //el vehiculo ha sido estacionado
        espera = (rand()% 25) +1;
        sleep(espera);
        pthread_mutex_lock(&mutex);
        fprintf(stderr,"Sale el coche %i\n", numCoche);
        plazasLibres++;
        desaparcar(plantaAux,plazaAux);
        muestraParking();
        //comprobamos que haya hueco en una plaza adyacente, teniendo cuidado con las plazas de los extremos
        if (((plazaAux < plazas-1) && (parking[plantaAux][plazaAux+1]==0)) || ((plazaAux>0) && (parking[plantaAux][plazaAux-1]==0))){
            pthread_cond_signal(&huecoCamion);
        }
        pthread_cond_signal(&no_lleno);
        pthread_mutex_unlock(&mutex);
    }
};

void aparcar(int *plantaAux, int *plazaAux, int numCoche){
    int i=0;
    int j;
    int aparcado=0;
    while (!aparcado && i<plantas){
        j=0;
        while (!aparcado && j<plazas){
            if (parking[i][j]==0){
                parking[i][j]=numCoche;
                aparcado=1;
            }
            j++;
        }
        i++;
    }
    *plazaAux=--j;
    *plantaAux=--i;
}
void desaparcar(int planta,int plaza){
    parking[planta][plaza]=0;
}


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
    while (1){
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
        fprintf(stderr,"Soy el camion %i\n", numCamion);        
        //Seccion crítica
        plazasLibres -= 2;
        aparcarCamion(plantaLibreAux,plazaLibreAux, numCamion);//la esta función busca un aparcamiento y devuelve la posición
        muestraParking();
        pthread_mutex_unlock(&mutex);//Liberamos el mutex para que otros puedan acceder al parking
        espera = (rand()% 8) +1;
        sleep(espera);//Esperamos un tiempo aleatorio
        pthread_mutex_lock(&mutex);//peleamos por el mutex para desaparcar
        fprintf(stderr,"Sale el camion %i\n", numCamion);        
        plazasLibres +=2;
        desaparcarCamion(plantaLibreAux, plazaLibreAux);
        pthread_cond_signal(&no_lleno);
        pthread_cond_signal(&huecoCamion);
        muestraParking();
        pthread_mutex_unlock(&mutex);
    }
};

void aparcarCamion(int plantaAux, int plazaAux, int numCamion){
    if(parking[plantaAux][plazaAux] || parking[plantaAux][plazaAux+1]){
        fprintf(stderr,"Err. Se está aparcando en una plaza que no se puede");
    }
    parking[plantaAux][plazaAux]=numCamion;
    parking[plantaAux][plazaAux+1]=numCamion;
}


void desaparcarCamion(int plantaAux, int plazaAux){
    parking[plantaAux][plazaAux]=0;
    parking[plantaAux][plazaAux+1]=0;
}

void muestraParking(){
    int i;
    int j;
    fprintf(stderr,"Parking:\n");
    for (i=0; i<plantas; i++){
        for (j=0; j<plazas; j++){
            fprintf(stderr,"[%d] ",parking[i][j]);
        }
        fprintf(stderr,"\n");
    }
}
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct vehiculo{
    int numero;
    int esCamion;//booleano que verifica si es camión o no
} TVehiculo;
typedef struct cola{
    TVehiculo *vehiculo;
    struct cola *sig;
}TCola;

//VARIABLES GLOBALES 
int **parking; //parking [planta][plaza]
int plazasLibres;
int plazas;
int plantas;
int coches;
int camiones;

TCola *cabezaTotal=NULL, *finTotal=NULL;
TCola *cabezaCamiones=NULL, *finCamiones=NULL;
TCola *aux;

int vehiculosCola;//Los vehiculos que están en la cola total
int vehiculosColaCamiones;//Los vehiculos que están en la cola de camiones
int contadorCoches;//Indica el numero de coches que han entrado desde el último camión;

pthread_mutex_t mutex;

//Tenemos la condición de que el parking está lleno
pthread_cond_t no_lleno;
pthread_cond_t huecoCamion;
pthread_cond_t turnoCamion;
//Vamos a darle una condición de espera para autorizarle a pasar en el caso de que sea el primero de la cola
pthread_cond_t *esperandoCoche;
pthread_cond_t *esperandoCamion;

void *coche();
void aparcar(int *planta, int *plaza, int numCoche);
void desaparcar(int planta,int plaza);
void *camion();
void aparcarCamion(int plantaAux, int plazaAux, int numCamion);
void desaparcarCamion(int plantaAux, int plazaAux);
int buscaLibre(int *plantaLibre, int *plazaLibre); //funcion que busca la plaza libre
void muestraParking();
void insertarTotal(TVehiculo *vehiculo);
void extraerTotal();
void insertarCamion(TVehiculo *vehiculo);
void extraerCamion();
TVehiculo *primeroTotal();
TVehiculo *primeroCamiones();

int main(int argc, char* argv[]){
    int i;
    int *nCoches;
    int *nCamiones;
    pthread_t tid;

    //Inicializamos las colas de camion y total
    // colaTotal = (TVehiculo *) malloc(sizeof(TVehiculo));
    // colaCamiones = (int *) malloc(sizeof(int));

    vehiculosCola = 0;
    vehiculosColaCamiones = 0;
    contadorCoches = 0;

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
    nCoches = (int *) calloc(coches, sizeof(int));//Inicializamos la lista de coches
    //Iniciamos el mutex y las condiciones
    pthread_mutex_init(&mutex, NULL); 
    pthread_cond_init(&no_lleno, NULL);
    pthread_cond_init(&huecoCamion, NULL);
    pthread_cond_init(&turnoCamion, NULL);

    // Inicializamos las listas de condiciones en funcion de los coches y camiones existentes
    esperandoCoche = (pthread_cond_t *) malloc(sizeof(pthread_cond_t)*coches);
    esperandoCamion = (pthread_cond_t *) malloc(sizeof(pthread_cond_t)*camiones);

    //creamos tantos threads como coches tenemos
    fprintf(stderr,"coches %i\n",coches);
    fprintf(stderr,"camion %i\n",camiones);

    for (i=0;i<coches;i++){
        nCoches[i] = (i+1);
        pthread_cond_init(&esperandoCoche[i], NULL);
        pthread_create(&tid,NULL,coche,(void *) &nCoches[i]);
    }
    //Creamos tantos threads como camiones tenemos
    nCamiones = (int *) calloc(camiones, sizeof(int));
    for (i=0;i<camiones;i++){
        nCamiones[i] = (101+i);
        pthread_cond_init(&esperandoCamion[i], NULL);
        pthread_create(&tid,NULL,camion,(void *) &nCamiones[i]);
    }
    while(1);
    return(1);
}

void *coche(void* nCoche){
    int numCoche = *(int *) nCoche;
    int plantaAux,plazaAux;
    int espera;
    TVehiculo primero;
    //Inicializamos el coche
    TVehiculo esteCoche;
    esteCoche.numero = numCoche;
    esteCoche.esCamion = 0;

    while (1){
        //Hacemos algo para que espere a entrar
        espera = (rand()% 40) +1;
        sleep(espera);
        pthread_mutex_lock(&mutex);//Tenemos el mutex bloqueado
        //Añadimos un coche a la cola total
        insertarTotal(&esteCoche);
        vehiculosCola++;
        primero = *primeroTotal();
        if(vehiculosCola == 1){//Puede darse la casualidad de que el parking esté lleno y llegue justo un coche, nadie le avisa
            while (!plazasLibres){
                pthread_cond_wait(&no_lleno, &mutex); //con esto va a esperar a que le avisen de que ya es el primero
            }
        }  
        else{
            while ((!plazasLibres) || !(primero.numero == esteCoche.numero && !primero.esCamion)){//espera mientras no sea el primero
                pthread_cond_wait(&esperandoCoche[numCoche-1], &mutex); //con esto va a esperar a que le avisen de que ya es el primero
                primero = *(TVehiculo *) primeroTotal();//En caso que se ejecute esta línea actualizo el primero, porque tengo el mutex
            }
        }
        //Seccion crítica
        //Lo quitamos de la cola porque volvemos a tener el control del mutex
        extraerTotal();
        vehiculosCola--;            
        fprintf(stderr,"Soy el coche %i\n", numCoche);  
        if(vehiculosCola){
            primero = *(TVehiculo *) primeroTotal();        
            fprintf(stderr,"Y el siguiente es %i\n", primero.numero);
        }
        plazasLibres--;
        aparcar(&plantaAux,&plazaAux, numCoche);//esta función busca un aparcamiento y devuelve la posición
        muestraParking();
        pthread_mutex_unlock(&mutex);
        //el vehiculo ha sido estacionado
        espera = (rand()% 40) +3;
        sleep(espera);
        pthread_mutex_lock(&mutex);
        fprintf(stderr,"Sale el coche %i\n", numCoche);
        plazasLibres++;
        desaparcar(plantaAux,plazaAux);
        muestraParking();
        fprintf(stderr,"Vehiculos %i\n\n", vehiculosCola);
        //Comprobamos que haya hueco en una plaza adyacente, teniendo cuidado con las plazas de los extremos
        
        pthread_cond_signal(&no_lleno);
        if(vehiculosCola){
            primero = *(TVehiculo *) primeroTotal();
            if(primero.esCamion){
                pthread_cond_signal(&esperandoCamion[(primero.numero)-1]);
            }
            else{
                pthread_cond_signal(&esperandoCoche[(primero.numero)-1]);
            }
        }
        if (((plazaAux < plazas-1) && (parking[plantaAux][plazaAux+1]==0)) || ((plazaAux>0) && (parking[plantaAux][plazaAux-1]==0))){
            pthread_cond_signal(&huecoCamion);
        }        
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
    int plantaLibreAux,plazaLibreAux;//Para que obtener el sitio que está libre en el parking
    int numCamion = *(int *)nCamion; //Numero del camión 
    int espera;
    TVehiculo primero;
    //Inicializamos el camion
    TVehiculo esteCamion;
    esteCamion.numero = numCamion;
    esteCamion.esCamion = 1;
    while (1){
        //Hacemos random para que espere a entrar
        espera = (rand()% 60) +1;
        sleep(espera);
        pthread_mutex_lock(&mutex); //Tenemos el mutex bloqueado
        //Añadimos un camion a la cola total
        insertarTotal(&esteCamion);
        vehiculosCola++;
        primero = *primeroTotal();
        /*Aqui ya hemos cubierto el caso de que llegue un camión cuando el parking está lleno
        puesto que no va a entrar en el primer while
        luego se pasará a la cola de camiones y le daremos el turno al camión
        */
        //
        //TT
        if(vehiculosCola == 1){//Puede darse la casualidad de que el parking esté lleno y llegue justo un coche, nadie le avisa
            while (!buscaLibre(&plantaLibreAux, &plazaLibreAux) ){
                pthread_cond_wait(&huecoCamion, &mutex); //con esto va a esperar a que le avisen de que ya es el primero
            }
        }  
        else{
            while (!buscaLibre(&plantaLibreAux, &plazaLibreAux) || !(primero.numero == esteCamion.numero && !primero.esCamion)){//espera mientras no sea el primero
                pthread_cond_wait(&esperandoCamion[numCamion-1], &mutex); //con esto va a esperar a que le avisen de que ya es el primero
                primero = *(TVehiculo *) primeroTotal();//En caso que se ejecute esta línea actualizo el primero, porque tengo el mutex
            }
        }     
        extraerTotal();
        vehiculosCola--; 
        contadorCoches = 0;
        //Seccion crítica
        fprintf(stderr,"Soy el camion %i\n", numCamion);  
        if(vehiculosCola){
            primero = *(TVehiculo *) primeroTotal();        
            fprintf(stderr,"Y el siguiente es %i\n", primero.numero);
        }  
        plazasLibres -= 2;
        aparcarCamion(plantaLibreAux,plazaLibreAux, numCamion);//la esta función busca un aparcamiento y devuelve la posición
        muestraParking();
        pthread_mutex_unlock(&mutex);//Liberamos el mutex para que otros puedan acceder al parking
        espera = (rand()% 60) +10;
        sleep(espera);//Esperamos un tiempo aleatorio
        pthread_mutex_lock(&mutex);//peleamos por el mutex para desaparcar
        fprintf(stderr,"Sale el camion %i\n", numCamion);        
        plazasLibres +=2;
        desaparcarCamion(plantaLibreAux, plazaLibreAux);
        pthread_cond_signal(&no_lleno);
        pthread_cond_signal(&huecoCamion);
        if(vehiculosCola){
            primero = *(TVehiculo *) primeroTotal();
            if(primero.esCamion){
                pthread_cond_signal(&esperandoCamion[(primero.numero)-1]);
            }
            else{
                pthread_cond_signal(&esperandoCoche[(primero.numero)-1]);
            }
        }
        muestraParking();
        fprintf(stderr,"Vehiculos %i\n\n", vehiculosCola);
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

void insertarTotal(TVehiculo *vehiculo){
    aux=(TCola *)malloc(sizeof(TCola));
    aux->vehiculo = vehiculo;
    aux->sig = NULL;
    if (cabezaTotal==NULL){
        finTotal=cabezaTotal=aux;
    }
        
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

void insertarCamion(TVehiculo *vehiculo){
    aux=(TCola *)malloc(sizeof(TCola));
    aux->vehiculo = vehiculo;
    aux->sig = NULL;
    if (cabezaCamiones==NULL)
        finCamiones=cabezaCamiones=aux;
    else
    {
        finCamiones->sig = aux;
        finCamiones = aux;
    }
}

void extraerCamion(){
    if(cabezaCamiones){
        aux = cabezaCamiones;
        cabezaCamiones = cabezaCamiones->sig;
        free(aux);
    }
}

TVehiculo *primeroTotal(){
    if(cabezaTotal){
        aux = cabezaTotal;
        return aux->vehiculo;
    }
    else return NULL;
}
TVehiculo *primeroCamiones(){
    if(cabezaCamiones){
        aux = cabezaCamiones;
        return aux->vehiculo;
    }
    else return NULL;
}
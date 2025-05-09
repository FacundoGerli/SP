/*Desarrolle un algoritmo paralelo que cuente la cantidad de veces que un 
elemento X aparece dentro de un vector de N elementos enteros.  Al finalizar,
 la cantidad de ocurrencias del elemento X debe quedar en una variable llamada
ocurrencias. Para la sincronización emplee mutex-locks. Pruebe con diversos tamaños de 
N y T={2,4,8}. Analice el rendimiento.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
int *A;
int N=100, T=1, size = 0, globalOcurrencias = 0, X = 1;
pthread_mutex_t mutexOcurrencias;
//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}
void *contOcurrencias(void* ptr){
    int id = *(int *) ptr;
    int inicio = id * size;
    int fin = inicio + size;
    int localOcurrencias = 0;

    if (id == T-1) {
        fin = N;
    }
    
    for(int i = inicio; i < fin; i++){
        if(A[i] == X)
        localOcurrencias ++;
    }
    pthread_mutex_lock(&mutexOcurrencias);
    globalOcurrencias += localOcurrencias;
    pthread_mutex_unlock(&mutexOcurrencias);
}
int main(int argc,char*argv[]){

    //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
     {
       printf("\nUsar: %s n t\n  n: Dimension del vector \n  t: Numero de hilos\n", argv[0]);
       exit(1);
     }

     int check=1;
     double timetick;
     int *IDS, i; 
     size = N/T;
    //Aloca memoria para el vector
    A=(int*)malloc(sizeof(int)*N);
    IDS =(int *)malloc(sizeof(int)*T);
    //Inicializa el vector
    for (i=0; i<N; i++){
        A[i] = 1;
    }

    //Crea los hilos
    pthread_t threads[T];
    pthread_mutex_init(&mutexOcurrencias, NULL);
     for(int i = 0; i < T; i++){
        IDS[i] = i;
        pthread_create(&threads[i], NULL, contOcurrencias,&IDS[i]);
      }
    
     //Realiza la multiplicacion./
    
      timetick = dwalltime();
      for( i=0; i < T; i++){
        pthread_join(threads[i], NULL);
      }

    
     printf("Conteo de ocurrencias en el vector de tamaño %d. Tiempo en segundos %f\n",N, dwalltime() - timetick);
     printf("Elemento %d encontrado %d veces\n", X, globalOcurrencias);

     free(A);
     free(IDS);
     pthread_mutex_destroy(&mutexOcurrencias);
     exit(0);
    }
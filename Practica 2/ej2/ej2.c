#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <sys/time.h>


//Dimension por defecto de las matrices
int N=100, blockSize, T=1;
double *A,*B,*C;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}
void* multmatriz(void* ptr){
    int suma;
    int id = *(int*) ptr;

    int inicio = id * blockSize;
    int fin = inicio + blockSize;
    if (id == T - 1) {
        fin = N;
    }
    for(int i = inicio; i < fin;i++){
        for(int j = 0; j < N; j++){
            suma = 0;
            for(int k = 0; k < N; k++){
                suma += A[i*N+k] * B[k*N+j];
            }
            C[i*N+j] = suma;
        }
    }
    pthread_exit(NULL);
}

int main(int argc,char*argv[]){
 int *IDS;
 int i,j,k;
 int check=1;
 double timetick;
 double suma;

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
  {
    printf("\nUsar: %s n t\n  n: Dimension de la matriz (nxn X nxn)\n  t: Numero de hilos\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  IDS =(int *)malloc(sizeof(int)*T);

  blockSize = N/T;
  pthread_t threads[T];

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    A[i*N+j]=1;
    B[j*N+i]=1;
   }
  }   
  
  for(int i = 0; i < T; i++){
    IDS[i] = i;
    pthread_create(&threads[i], NULL, multmatriz,&IDS[i]);
  }

 //Realiza la multiplicacion./

  timetick = dwalltime();
  for( i=0; i < T; i++){
    pthread_join(threads[i], NULL);
  }

 printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n",N,N, dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(C[i*N+j]==N);
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(IDS);
 free(A);
 free(B);
 free(C);
 return(0);
}
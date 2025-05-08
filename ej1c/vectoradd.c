#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

double *A,*B,*C;
int *ID;
int blockSize,extra,threads;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}
void *sumvector(void *arg){
  int id = *(int*)arg;
  int inicio = id * blockSize;
  int fin = inicio + blockSize;
  if (id == threads - 1) {
    fin += extra;
  }
  for(int i = inicio; i<fin; i++){
    C[i] = A[i] + B[i];
  }
  pthread_exit(NULL);
}

int main(int argc,char*argv[]){
int N;

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((threads = atoi(argv[2])) < 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

  int i,j,k;
  int check=1;
  double timetick;

 //Aloca memoria para los vectores
  A=(double*)malloc(sizeof(double)*N);
  B=(double*)malloc(sizeof(double)*N);
  C=(double*)malloc(sizeof(double)*N);
  ID=(int *)malloc(sizeof(int)*threads);

  pthread_t thread_handle[threads];
  blockSize = N/threads;
  printf("blockSize: %d\n", blockSize);
  extra = N % threads;

 //Inicializa los vectores en 1, el resultado será una vector con todos sus valores en 2
  for(i=0;i<N;i++)
	A[i] = 1;
  for(i=0;i<N;i++)
	B[i] = 1;

  for (i=0; i < threads;i++) {
    ID[i] = i;
    pthread_create(&thread_handle[i], NULL, sumvector, &ID[i]);
  }
  timetick = dwalltime();
  for (i=0; i < threads;i++) {
    pthread_join(thread_handle[i], NULL);
  }
  double time = dwalltime() - timetick;


 printf("Suma de vectores de dimensión %d. Tiempo en segundos %f\n",N,time);

 //Verifica el resultado
  for(i=0;i<N;i++)
	check=check&&(C[i]==2);
   
      if(check){
   printf("Suma de vectores resultado correcto\n");
  }else{
   printf("Suma de vectores resultado erroneo\n");
  }

 free(A);
 free(B);
 free(C);
 free(ID);
 return(0);
}

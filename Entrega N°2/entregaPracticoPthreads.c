#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<pthread.h>
#include <limits.h>

#define blockSize 64
  
int N = 10, T = 1, longHilo = 0;
double *A, *B, *C, *R, *BT,*resultMatriz;
double minA = INT_MAX, minB = INT_MAX, maxA = INT_MIN, maxB = INT_MIN, promA = 0,promB = 0, escalar = 0;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

double dwalltime();
void matmulblks(double *a, double *b, double *c, int n, int bs, int inicio, int fin);
void blkmul(double *Rlk, double *bblk, double *cblk, int n, int bs);
void initvalmat(double *mat, int n, double val, int transpose);
void transponerblks(double *matriz, double *matrizT, int blocksize, int inicio, int fin);

void *job(void *arg){
    int id = *(int *) arg;
    int inicio = id * longHilo;
    int fin = inicio + longHilo;

    if( id == T - 1){
        fin = N;
    }

    double localSumA = 0, localSumB = 0, localMinA = 9999, localMaxA = -1, localMinB = 9999, localMaxB = -1;
    double posA, posB;
    int desplazamientoI;

    for (int i = inicio; i < fin; i++){
        desplazamientoI = i * N;
        for(int j = 0; j < N; j++){
            posA = A[desplazamientoI + j];
            posB = B[desplazamientoI + j];
            if(posA < localMinA){
                localMinA = posA;
            }
            if(posA > localMaxA){
                localMaxA = posA;
            }
            if(posB < localMinB){
                localMinB = posB;
            }
            if(posB > localMaxB){
                localMaxB = posB;
            }
            localSumA += posA;
            localSumB += posB;
        }
    }

    pthread_mutex_lock(&mutex);
    if(localMaxA > maxA){
        maxA = localMaxA;
    }
    if(localMinA < minA){
        minA = localMinA;
    }
    if(localMaxB > maxB){
        maxB = localMaxB;
    }
    if(localMinB < minB){
        minB = localMinB;
    }
    promA += localSumA;
    promB += localSumB;
    pthread_mutex_unlock(&mutex);

    pthread_barrier_wait(&barrier);

    if(id == 0){
        promA = promA / (N*N);
        promB = promB / (N*N);
        escalar = (maxA *maxB - minA * minB) / (promA * promB);
    }
    transponerblks(B,BT, blockSize, inicio, fin);
    matmulblks(A, B, R, N, blockSize, inicio, fin);
    matmulblks(C, BT, resultMatriz, N, blockSize, inicio, fin);
    pthread_barrier_wait(&barrier);
    
    for (int i = inicio; i < fin; i++){
        desplazamientoI = i * N;
        for (int j = 0; j < N; j++){
            resultMatriz[desplazamientoI + j] += R[desplazamientoI + j] * escalar;
        }
    }
    pthread_exit(0);
}
int main(int argc, char *argv[]) {

    // Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0)
     || ((N % blockSize) != 0)) {
        printf("No se ha ingresado un argumento o el valor ingresado es invalido \n");
        exit(1);
    }
    printf("Se han ingresado el argumento: N: %i T: %i\n", N, T);

    double timetick;
    int desplazamientoI, i, desplazamientoJ, desplazamientoBI;
    int ids[T];
    longHilo = N/T;
    pthread_t threads[T];

    // Alocar matriz 
    int size = N * N;
     A = (double *) malloc(size * sizeof(double));
     B = (double *) malloc(size * sizeof(double));
     C = (double *) malloc(size * sizeof(double));
     BT = (double *) malloc(size * sizeof(double)); 
     R = (double *) malloc(size * sizeof(double));
     resultMatriz = (double *) malloc(size * sizeof(double));


    initvalmat(A, N, 1.0, 0);
    initvalmat(B, N, 1.0, 1);
    initvalmat(C, N, 1.0, 0); 
    //Resultados
    initvalmat(R, N, 0, 0);
    initvalmat(resultMatriz, N, 0, 0);

    pthread_barrier_init(&barrier, NULL, T);
    pthread_mutex_init(&mutex, NULL);

       // Iniciar contador tiempo
    timetick = dwalltime();

    for (i = 0; i < T; i++){
        ids[i] = i;
        pthread_create(&threads[i], NULL, job, &ids[i]);
    }

    for(i=0; i <T; i++){
        pthread_join(threads[i], NULL);
    }
    // Finalizar contador tiempo
    printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);


    // Liberar memoria
    free(A);
    free(B);
    free(C);
    free(BT);
    free(R);
    free(resultMatriz);
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    return 0;
}

#include <stdio.h>
#include <sys/time.h>

double dwalltime() {
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}


void initvalmat(double *mat, int n, double val, int transpose)
{
  int desplazamientoI;
  int i, j;      /* Indexes */

	if (transpose == 0) { //Por fila
	  for (i = 0; i < n; i++)
	  {
      desplazamientoI = i * N;
		  for (j = 0; j < n; j++)
		  {
		    mat[desplazamientoI + j] = val;
		  }
	  }
	} else { //por columna
	  for (i = 0; i < n; i++)
	  {
		for (j = 0; j < n; j++)
		{
		  mat[j*n + i] = val;
		}
	  }
	}
}

void transponerblks(double *matriz, double *matrizT, int bs, int inicio, int fin){
    int i, j, desplazamientoBI, bj;
    for (i = inicio; i < fin; i += bs){
        for(j = 0; j < N; j += bs){
          for (desplazamientoBI = 0; desplazamientoBI < bs && (i + desplazamientoBI) < N; desplazamientoBI++){
            for(bj = 0; bj < bs && (j + bj) < N; bj++){
                matrizT[(j + bj) * N + (desplazamientoBI + i)] = matriz[(desplazamientoBI + i) * N + (j + bj)];
            }
          }
        }
    }
    }

/* Multiply square matrices, blocked version */
void matmulblks(double *a, double *b, double *c, int n, int bs, int inicio, int fin)
{
  int i, j, k, desplazamientoI, desplazamientoJ;    /* Guess what... */
  
  for (i = inicio; i < fin; i += bs)
  {
    desplazamientoI = i * n;
    for (j = 0; j < n; j += bs)
    {
      desplazamientoJ = j * n;
      for  (k = 0; k < n; k += bs)
      {
        blkmul(&a[desplazamientoI + k], &b[desplazamientoJ + k], &c[desplazamientoI + j], n, bs);
      }
    }
  }
}

/*****************************************************************/

/* Multiply (block)submatrices */
void blkmul(double *Rlk, double *bblk, double *cblk, int n, int bs)
{
  int i, j, k, desplazamientoI, desplazamientoJ;;    /* Guess what... again... */
  double suma;

  for (i = 0; i < bs; i++)
  {
    desplazamientoI = i * n;
    for (j = 0; j < bs; j++)
    {
      suma = 0;
      desplazamientoJ = j * n;
      for  (k = 0; k < bs; k++)
      {
        suma += Rlk[desplazamientoI + k] * bblk[desplazamientoJ + k];
      }
      cblk[desplazamientoI + j] += suma;
    }
  }
}
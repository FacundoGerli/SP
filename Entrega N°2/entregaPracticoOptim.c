#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>

#define blockSize 64
  
int N = 10; 

double dwalltime();
void matmulblks(double *a, double *b, double *c, int n, int bs);
void blkmul(double *Rlk, double *bblk, double *cblk, int n, int bs);
void initvalmat(double *mat, int n, double val, int transpose);
void imprimirMatriz(double *matriz, int N);
void minMaxProm(double *matriz, int N, double *min, double *max, double *prom);
void transponer(double *matriz, int N, double *matrizT, int transpose);

int main(int argc, char *argv[]) {

    double timetick;
    double *A, *B, *C, *R, *BT,*resultMatriz;
    double minA,minB,maxA,maxB,promA,promB;
    int desplazamientoI, desplazamientoJ, desplazamientoBI;

    // Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) || ((N % blockSize) != 0)) {
        printf("No se ha ingresado un argumento o el valor ingresado es invalido \n");
        exit(1);
    }
    printf("Se han ingresado el argumento: N: %i\n", N);

    // Alocar matriz 
    double size = N * N;
     A = (double *) malloc(size * sizeof(double));
     B = (double *) malloc(size * sizeof(double));
     C = (double *) malloc(size * sizeof(double));
     BT = (double *) malloc(size * sizeof(double)); 
     R = (double *) malloc(size * sizeof(double));
     resultMatriz = (double *) malloc(size * sizeof(double));


    initvalmat(A, N, 1.0, 0);
    initvalmat(B, N, 1.0, 1);
    initvalmat(C, N, 1.0, 0); 
    initvalmat(BT, N, 0, 1);
    //Resultados
    initvalmat(R, N, 0, 0);
    initvalmat(resultMatriz, N, 0, 0);

    //imprimirMatriz(A, N);
   // imprimirMatriz(B, N);
   // imprimirMatriz(C, N);
    //imprimirMatriz(BT, N);
    //imprimirMatriz(resultMatriz, N);

    // Iniciar contador tiempo
    timetick = dwalltime();

    transponer(B, N, BT, 0);
    minMaxProm(A, N, &minA, &maxA, &promA);
    minMaxProm(B, N, &minB, &maxB, &promB);
    double escalar = (maxA * maxB - minA * minB) / (promA * promB);
    matmulblks(A, B, R, N, blockSize);
    matmulblks(C, BT, resultMatriz, N, blockSize);

    
    for (int i = 0; i < N; i++){
        desplazamientoI = i * N;
        for(int j = 0; j < N; j++){
            resultMatriz[desplazamientoI + j] += R[desplazamientoI + j] * escalar;
        }
    }

    // Finalizar contador tiempo
    printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);

   // printf("Matriz resultado ->\n");
 //   imprimirMatriz(resultMatriz, N);

    // Liberar memoria
    free(A);
    free(B);
    free(C);
    free(BT);
    free(resultMatriz);
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

void imprimirMatriz(double *matriz, int N) {
    int desplazamientoI;
    printf("Matriz ->");
    printf("\n");
    for (int i = 0; i < N; i++) {
        desplazamientoI = i * N;
        for (int j = 0; j < N; j++) {
            printf("|%.0f|", matriz[ desplazamientoI + j]);
        }
        printf("\n");
    }
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
void minMaxProm(double *matriz, int N, double *min, double *max, double *prom){
  int desplazamientoI;
    *min = 100;
    *max = -1;
    *prom = 0;
 //Define la forma de recorrer la matriz de entrada
        for (int i = 0; i<N; i++){
          desplazamientoI = i * N;
            for (int j=0; j<N; j++){
                if(matriz[desplazamientoI + j] < *min){
                    *min = matriz[desplazamientoI + j];
                }
                if(matriz[desplazamientoI + j] > *max){
                    *max = matriz[desplazamientoI + j];
                }
                *prom += matriz[desplazamientoI + j];
            }
        }
    
    
    *prom = *prom/(N*N);
}

void transponer(double *matriz, int N, double *matrizT, int transpose){
    int desplazamientoBI;
  //Define la forma de recorrer la matriz de entrada
  //0 -> por fila, 1 -> por columna
    if(transpose == 0){
        for(int i = 0; i < N; i += blockSize){
            for(int j = 0; j < N; j += blockSize){

                for(int bi = 0; bi < blockSize && (i+bi) < N; bi++){
                  desplazamientoBI = i+bi;
                    for(int bj = 0; bj <blockSize && (j+bj) < N; bj++){
                        matrizT[(j+bj) * N + (desplazamientoBI)] = matriz[(desplazamientoBI) * N + (j+bj)];
                    }
                }
            }
        }
    }
    else {
        for(int i = 0; i < N; i += blockSize){
            for(int j = 0; j < N; j += blockSize){

                for(int bi = 0; bi < blockSize && (i+bi) < N; bi++){
                  desplazamientoBI = i+bi;
                    for(int bj = 0; bj <blockSize && (j+bj) < N; bj++){
                        matrizT[(desplazamientoBI) * N + (j+bj)] = matriz[(j+bj)* N + (desplazamientoBI)];
                    }
                }
            }
        }

    }

}
/* Multiply square matrices, blocked version */
void matmulblks(double *a, double *b, double *c, int n, int bs)
{
  int i, j, k, desplazamientoI, desplazamientoJ;    /* Guess what... */

  /* Init matrix c, just in case */  
  initvalmat(c, n, 0.0, 0);
  
  for (i = 0; i < n; i += bs)
  {
    desplazamientoI = i * N;
    for (j = 0; j < n; j += bs)
    {
      desplazamientoJ = j * N;
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
    desplazamientoI = i * N;
    for (j = 0; j < bs; j++)
    {
      suma = 0;
      desplazamientoJ = j * N;
      for  (k = 0; k < bs; k++)
      {
        suma += Rlk[desplazamientoI + k] * bblk[desplazamientoJ + k];
      }
      cblk[desplazamientoI + j] += suma;
    }
  }
}
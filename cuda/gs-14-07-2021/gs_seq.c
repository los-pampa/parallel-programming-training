/*gcc gs_seq.c -o gs_seq -pg -O3 -lm -fopenmp 
  gprof gs_seq gmon.out > analise.txt
  export OMP_DISPLAY_ENV=TRUE
  export OMP_NUM_THREADS=64
  export OMP_PROC_BIND=close, spread, false, true, master
  export OMP_PLACES=cores, sockets, threads
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define ROWS 2048
#define COLS 2048


// OMP/serial code for performance testing
void gramSchmidt(float *Q, int rows, int cols){
    float *Qt = malloc(sizeof(float)*cols*rows);
    int i, j, k; // i=0 - j=0 - k=0
    float tmp;
    int chunksize = rows/128;
    /* Transposição da matriz Q para Qt */
    #pragma omp parallel for
    for (i = 0; i < rows; i++){ 
          for (j = 0; j < cols; j++){
                Qt[(j*rows)+i] = Q[(i*rows)+j];
          }
    }

    for(k=0; k < rows; k++) {
            tmp = 0.;
            #pragma omp parallel for simd reduction(+:tmp) schedule(static,chunksize) //guided, dynamic
            for(i=0; i < cols; i++) // Somatorio do produto
                  tmp +=  (Qt[(k*cols)+i] * Qt[(k*cols)+i]);
            tmp = sqrt(tmp);

            // Tem como fazer reduction com vetor?
            #pragma omp parallel for simd schedule(static,chunksize) //guided, dynamic
	      for(i=0; i < cols; i++) 
			Qt[(k*cols)+i] /= tmp;//

            #pragma omp parallel for simd private(i) schedule(static, chunksize)
            for(j=k+1; j < rows; j++) {
                  tmp=0.;
                  // #pragma omp parallel for reduction(+:tmp)
                  for(i=0; i < cols; i++) 
                        tmp += Qt[(k*cols)+i] * Qt[(j*cols)+i];
                  for(i=0; i < cols; i++) 
                        Qt[(j*cols)+i] -= tmp * Qt[(k*cols)+i];
            }
	}

      #pragma omp parallel for
      for (i = 0; i < rows; i++){ 
            for (j = 0; j < cols; j++){
                        Q[(i*rows)+j] = Qt[(j*rows)+i];
            }
      }
}


void checkOctave(float *A, int rows, int cols){
  int found_error=0;
  int c1, c2, i;
  float sum;
  for(c1=0; c1 < cols; c1++){
        for(c2=c1; c2 < cols; c2++) {
              sum=0.;
              for(i=0; i < rows; i++) 
                    sum += A[(i*rows)+c1] * A[(i*rows)+c2];
              if(c1 == c2) { // should be near 1 (unit length)
                    if(sum < 0.9) {
                            printf("Failed unit length: %d %d %g\n", c1,c2,sum);
                            found_error = 1;
                            exit(1);
                    }
              }else{ // should be very small (orthogonal)
                    if(sum > 0.1) {
                            printf("Failed orthonogonal  %d %d %g\n", c1,c2,sum);
                            found_error = 1;
                            exit(1);
                   }
              }
        }
    }
      if(!found_error) printf("Check OK!\n");
}


int main(int argc, char *argv[]){

    int i;   

    int cols=COLS, j, rows=ROWS;
    float *A = malloc(sizeof(float)*rows*cols);
    // fill matrix A with random numbers
    for(i=0; i < rows; i++)
          for(j=0; j < cols; j++)
                A[(i*rows)+j] = (float)rand()/ (float)RAND_MAX;

    //printOctave("A", A, rows, cols);
    double t1 = omp_get_wtime();
    gramSchmidt(A,rows, cols);
    double t2 = omp_get_wtime();
    printf("%.15f\n", t2-t1);
    checkOctave(A, rows, cols);
    free(A);

    return 0;

}

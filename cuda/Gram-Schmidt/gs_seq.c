#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define ROWS 2048
#define COLS 2048

// OMP/serial code for performance testing
void gramSchmidt(float *Q, int rows, int cols){
    float *Qt = malloc(sizeof(float)*cols*rows);
    int i, j, k;
    float tmp;
    for(k=0; k < cols; k++) {
          tmp = 0.;
                for(i=0; i < rows; i++) 
                        tmp +=  (Q[(i*rows)+k] * Q[(i*rows)+k]);
          tmp = sqrt(tmp);
				for(i=0; i < rows; i++) 
						Q[(i*rows)+k] /= tmp;
              for(j=k+1; j < cols; j++) {
                    tmp=0.;

                    for(i=0; i < rows; i++) 
                          tmp += Q[(i*rows)+k] * Q[(i*rows)+j];
                    for(i=0; i < rows; i++) 
                          Q[(i*rows)+j] -= tmp * Q[(i*rows)+k];
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

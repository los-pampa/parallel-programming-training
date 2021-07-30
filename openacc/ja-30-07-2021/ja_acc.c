#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>
//#include <omp.h>

#define N 4096
#define ITER_MAX 2000

void print_matrix(double *Anew){
    for (int i = 0; i < N; i++){
        for (int j = 0; j <N; j++){
            printf("%lf\n", Anew[(i*N)+j]);
        }
    }
}

int main(int argc, char** argv){
    int i, j;
    int iter = 0;
    const double pi = 2.0f * asinf(1.0f);
    double *A = malloc(sizeof(double)*(N*N));
    double *Anew = malloc(sizeof(double)*(N*N));
    double *y0 = malloc(sizeof(double)*N);
    memset(A, 0, N * N * sizeof(double));
    for(i = 0; i < N; i++){
        A[i]   = 0.f;
        A[((N-1)*N)+i] = 0.f;
        y0[i] = sinf(pi * i / (N-1));
        A[(i*N)] = y0[i];
        A[(i*N)+N-1] = y0[i]*expf(-pi);
        Anew[i] = 0.f;
        Anew[((N-1)*N)+i] = 0.f;
        Anew[i*N] = y0[i];
        Anew[(i*N)+N-1] = y0[i]*expf(-pi);
    }
        #pragma acc data copy(A[0:N*N]) copyin(Anew[0:N*N])
        {
            for(iter=0;iter<ITER_MAX;iter++)
            {
                #pragma acc parallel loop gang num_gangs(256) vector vector_length(1024) collapse(2) private(j, i)
                for(j = 1; j < N-1; j++){
                    for(i = 1; i < N-1; i++ ){
                        Anew[(j*N)+i] = 0.25f * (A[(j*N)+i+1] + A[(j*N)+i-1]+ A[((j-1)*N)+i] + A[((j+1)*N)+i]);
                    }
                }

                #pragma acc parallel loop gang num_gangs(256) vector vector_length(1024) collapse(2) private(j, i)
                for(j = 1; j < N-1; j++){
                    for(i = 1; i < N-1; i++ ){
                        int aux = A[(j*N)+i];
                        A[(j*N)+i] = Anew[(j*N)+i];
                        Anew[(j*N)+i] = aux;
                    }
                }  
            }
        }
        print_matrix(A);
        if(A[5] == 0.45)
            printf("error\n");
    return 0;
}

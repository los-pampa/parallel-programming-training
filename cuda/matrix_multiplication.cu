#include <stdio.h>
#include <stdlib.h>

#define N 4096
#define M 4096
#define K 4096

void cpu_matrix_multiplication(float *A, float *B, float *C){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            for(int k = 0; k<K; k++){
                C[(i*N)+j] += A[(i*M)+k] * B[(k*N)+j];
            }
        }
    }
}

void print_matrix(float *C) {
    for(int i=0; i< N; i++) {
        for(int j=0; j<M; j++){
            printf("%f ", C[(i*N)+j]);
        }
        printf("\n");
    }
}


__global__ void gpu_matrix_multiplication(int *A, int *B, int *C){
    int linha = blockIdx.y * blockDim.y + threadIdx.y;
    int coluna = blockIdx.x * blockDim.x + threadIdx.x;
    for(int k = 0; k<K; k++){
        C[(linha*N)+coluna] += A[(linha*M)+k] * B[(k*N)+coluna];
    }

}

int main(int argc, char **argv) {
    float *A, *B, *C;
    float *A_d, *B_d, *C_d;
    /* Alocação na CPU */
    A = (float*) malloc(sizeof(float)*M*K);
    B = (float*) malloc(sizeof(float)*K*N);
    C = (float*) malloc(sizeof(float)*N*M);

    /* Alocação na GPU */
    cudaMalloc(&A_d, M*K*sizeof(float));
    cudaMalloc(&B_d, K*N*sizeof(float));
    cudaMalloc(&C_d, M*N*sizeof(float));

    /*preenche as matrizes com valores iniciais */
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            A[(i*N)+j] = (float) i;
            B[(i*N)+j] = (float) j;
            C[(i*N)+j] = 0.0;
        }
    }


    /* Copiar dos da CPU para a GPU */
    cudaMemcpy(A_d, A, M*K*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(B_d, B, K*N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(C_d, C, M*N*sizeof(float), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(32,32)
    dim3 blocksPerGrid(N/threadsPerBlock.x, N/threadsPerBlock.y)

    gpu_matrix_multiplication<<<blocksPerGrid, threadsPerBlock>>>();
    //cpu_matrix_multiplication();

    cudaMemcpy(C, C_d, M*N*sizeof(float), cudaMemcpyDeviceToHost);

    print_matrix(C);
    
    cudaFree(A_d);
    cudaFree(B_d);
    cudaFree(C_d);
    
    free(A);
    free(B);
    free(C);
}

















































    gpu_matrix_multiplicacao<gridXblocoYY>>>(A, B, C);    // 

    cpu_matrix_multiplication(A, B, C);

    print_matrix(C);
    return 0;
}
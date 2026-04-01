#include <stdio.h>
#include <stdlib.h>
#include "mpi.h" 

void print_vetor(double* vet, int n){
    for (int i = 0; i < n; i++)
    {
        printf("%.2f   ", vet[i]);
    }
    
    return;
}

void print_matriz(double* A, int m, int n){

    for (int i = 0; i < m; i++){
        printf("\n");

        print_vetor(&A[i*n], n);
            
        }
    
    printf("\n\n");
    return;
}

void mxv(double* temporaria, double* b, double* c, int n, int linhas_por_proc){

    for (int i = 0; i < linhas_por_proc; i++)
    {   
        c[i] = 0;
        for (int j = 0; j < n; j++)
        {
            c[i] += temporaria[i*n + j]*b[j];
        }
    }

}


int main(int argc, char* argv[]){

    int m = 4; //8 ou 16
    int n = 5;
    int meu_ranque;
    int num_procs;
    double* A;
    double* result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int linhas_por_proc = m/num_procs;

    //Todos os processos criam e inicializam B 
    double* b = malloc(n*sizeof(double));

    for (int j = 0; j < n; j++)
              b[j] = 2.0;

    double* temporaria = malloc(linhas_por_proc*n*sizeof(double));
    double* c = malloc(linhas_por_proc*sizeof(double));

    //Só Proc 0 guarda a matriz A
    if(meu_ranque == 0){
        A = malloc(m*n*sizeof(double));
        result = malloc(m*sizeof(double));

    }



    if (meu_ranque == 0) {
        //  printf("Atribuindo valor inicial à matriz A e ao vetor b\n");

         
         for (int i = 0; i < m; i++)
              for (int j = 0; j < n; j++)
                  A[i*n + j] = (double) i;
     }

    

    MPI_Scatter(A, n*linhas_por_proc, MPI_DOUBLE, temporaria, n*linhas_por_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();


    //Aqui cada processo tem um pedaço da matriz A em temporaria, o vetor b e o vetor resultado c

    mxv(temporaria, b, c, n, linhas_por_proc);
    
    
    MPI_Gather(c, linhas_por_proc, MPI_DOUBLE, result, linhas_por_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double finish = MPI_Wtime();


    if(meu_ranque == 0){

        printf("Valor de m: %d\nValor de n: %d\n\n", m, n);

        printf("\nTempo total = %e\n\n", finish - start);
        printf("Matriz A:");
        print_matriz(A, m, n);

        printf("\nVetor B:\n");
        print_vetor(b, n);

        printf("\n\nVetor C:\n");
        print_vetor(result, m);
        printf("\n\n");
    }
    
    MPI_Finalize();
    


    return 0;
}
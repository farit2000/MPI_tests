#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 10

int main (int argc, char *argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);
    int rem = SIZE % ProcNum;
    int shag = SIZE / ProcNum;
    double rec_buf_root[rem + shag];
    double rec_buf_other[shag];
    //Распределение

    if (ProcRank == 0) {
        double arr[SIZE];
        srand(time(NULL));
        for (int i = 0; i < SIZE; i++) {
            arr[i] = rand() % 21;
            printf("%f\t", arr[i]);
        }

        for (int i = 0; i < shag + rem; i++)
            rec_buf_root[i] = arr[i];
        int j = rem + shag;
        for(int i = 1; i < ProcNum; i++) {
            MPI_Send(&arr[j], shag, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
            j += shag;
        }
        printf("\n");
        printf("Rank is : %d\n", ProcRank);
        for (int i = 0; i < shag + rem; i++) {
            printf("%f\t", rec_buf_root[i]);
        }
    } else {
        MPI_Recv(&rec_buf_other[0], shag, MPI_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
        printf("\n");
        printf("Rank is : %d\n", ProcRank);
        for(int i = 0; i < shag; i++) {
            printf("%f\t", rec_buf_other[i]);
        }
    }

    //Сборка

    if (ProcRank == 0) {
        double res[SIZE];
        for (int i = 0; i < shag + rem; i++)
            res[i] = rec_buf_root[i];
        int j = shag + rem;
        for (int i = 1; i < ProcNum; i++) {
            MPI_Recv(&res[j], shag, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            j += shag;
        }
        printf("\n");
        printf("Result : \n");
        for(int i = 0; i < SIZE; i++) {
            printf("%f\t", res[i]);
        }
    } else {
        MPI_Send(&rec_buf_other[0], shag, MPI_DOUBLE, 0, ProcRank, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
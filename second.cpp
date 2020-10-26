#include <stdio.h>
#include <iostream>
#include "mpi.h"

void DataInitialization(double *x, int N) {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        x[i] = rand() % 21;
    }
}

int main (int argc, char *argv[]) {
    double *x = new double[10];
    double TotalMax, ProcMax = -1000;
    int ProcRank, ProcNum, N=10;
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);

    if ( ProcRank == 0 )
        DataInitialization(x, N);

    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    int k = N / ProcNum;
    int i1 = k * ProcRank;
    int i2 = k * ( ProcRank + 1 );
    if ( ProcRank == ProcNum-1 ) i2 = N;
    for ( int i = i1; i < i2; i++ )
    {
        if (x[i] > ProcMax) ProcMax = x[i];
    }

    MPI_Reduce(&ProcMax,&TotalMax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if ( ProcRank == 0 ) {
        for (int i = 0; i < 10; i++)
            printf("%f\n", x[i]);
        printf("\nTotal Max = %10.2f", TotalMax);
    }
    MPI_Finalize();

    delete [] x;
    return 0;
}
#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 4
int main (int argc, char *argv[]) {
    int rank, size, rem, sum = 0;
    double rec_buf[100], TotalSum, ProcSum = 0.0;

    double data[SIZE][SIZE] = {
            {1.0, 3.0, 6.0, 2.0},
            {5.0, 9.0, 2.0, 5.0},
            {4.0, 7.0, 8.0, 5.0},
            {3.0, 4.0, 0.0, 1.0}
    };

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    rem = (SIZE*SIZE)%size;
    int *sendcounts = new int[size];
    int *displs = new int[size];

    for (int i = 0; i < size; i++) {
        sendcounts[i] = (SIZE*SIZE)/size;
        if (rem > 0) {
            sendcounts[i]++;
            rem--;
        }

        displs[i] = sum;
        sum += sendcounts[i];
    }

    MPI_Scatterv(&data, sendcounts, displs, MPI_DOUBLE, &rec_buf, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < sendcounts[rank]; i++) {
        ProcSum = ProcSum + rec_buf[i];
    }

    MPI_Reduce(&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (0 == rank) {
        printf("Result is : %f", TotalSum/16);
    }

    MPI_Finalize();

    return 0;
}
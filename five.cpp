#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 4
int main (int argc, char *argv[]) {
    int rank, size, rem, sum = 0;
//    int rem = (SIZE*SIZE)%size; // elements remaining after division among processes
//    int sum = 0;                // Sum of counts. Used to calculate displacements
    double rec_buf_x[100], rec_buf_y[100], TotalSum, ProcSum = 0.0;

    double x[SIZE][SIZE] = {
            {1.0, 3.0, 6.0, 2.0},
            {5.0, 9.0, 2.0, 5.0},
            {4.0, 7.0, 8.0, 5.0},
            {3.0, 4.0, 0.0, 1.0}
    };

    double y[SIZE][SIZE] = {
            {1.0, 3.0, 6.0, 2.0},
            {5.0, 9.0, 2.0, 5.0},
            {4.0, 7.0, 8.0, 5.0},
            {3.0, 4.0, 0.0, 1.0}
    };

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    rem = (SIZE*SIZE)%size;


    for (int i = 0; i < size; i++) {
        sendcounts[i] = (SIZE*SIZE)/size;
        if (rem > 0) {
            sendcounts[i]++;
            rem--;
        }

        displs[i] = sum;
        sum += sendcounts[i];
    }

    if (0 == rank) {
        for (int i = 0; i < size; i++) {
            printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
        }
    }

    MPI_Scatterv(&x, sendcounts, displs, MPI_DOUBLE, &rec_buf_x, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&y, sendcounts, displs, MPI_DOUBLE, &rec_buf_y, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < sendcounts[rank]; i++) {
        ProcSum = ProcSum + rec_buf_x[i] * rec_buf_y[i];
    }

    MPI_Reduce(&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (0 == rank) {
        printf("Result is : %f", TotalSum);
    }

    MPI_Finalize();

    return 0;
}
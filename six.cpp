#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 4
int main (int argc, char *argv[]) {
    int rank, size, rem, sum = 0;
    double rec_buf[100], TotalMaxMin, ProcMax = 1000, ProcMin = -1000.0;

    double data[SIZE][SIZE] = {
            {5.0, 10.0, 6.0, 7.0},
            {5.0, 9.0, 4.0, 5.0},
            {5.0, 7.0, 8.0, 6.0},
            {3.0, 4.0, 6.0, 1.0}
    };

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    rem = SIZE % size;
    int *sendcounts = new int[size];
    int *displs = new int[size];

    for (int i = 0; i < size; i++) {
        sendcounts[i] = (SIZE / size) * SIZE;
        if (rem > 0) {
            sendcounts[i] += SIZE;
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

    MPI_Scatterv(&data, sendcounts, displs, MPI_DOUBLE, &rec_buf, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    printf("%d: ", rank);
    for (int i = 0; i < sendcounts[rank]; i++) {
        printf("%f\t", rec_buf[i]);
    }
    printf("\n");

    if (sendcounts[rank] > SIZE) {
        int i1 = 0;
        double *mins = new double[sendcounts[rank] / SIZE];
        for (int i = 0; i < sendcounts[rank] / SIZE; i++) {
            mins[i] = 10000;
        }
        int i2 = SIZE;
        for (int i = 0; i < (sendcounts[rank] / SIZE); i++){
            double min = 10000.0;
            for (int i1 = 0; i1 < i2; i1++) {
                if (rec_buf[i1] < mins[i])
                {
                    mins[i] = rec_buf[i1];
                }
            }
            i2 = i2 + SIZE;
        }
        for (int i = 0; i < sendcounts[rank] / SIZE; i++) {
            if (mins[i] > ProcMin) ProcMin = mins[i];
        }
        MPI_Reduce(&ProcMin, &TotalMaxMin, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    }
    else {
        for (int i = 0; i < sendcounts[rank]; i++) {
            if (rec_buf[i] < ProcMax) ProcMax = rec_buf[i];
        }
        MPI_Reduce(&ProcMax, &TotalMaxMin, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    }

    if (0 == rank) {
        printf("Result is : %f", TotalMaxMin);
    }

    MPI_Finalize();
    free(sendcounts);
    free(displs);

    return 0;
}
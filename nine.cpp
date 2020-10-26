#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 7

int main (int argc, char *argv[]) {
    int ProcNum, ProcRank;
    double arr[SIZE];
//    double rec_buf[100];
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);
    int rem = SIZE % ProcNum;
    int shag = SIZE / ProcNum;
    int q = SIZE;
    if (ProcRank == 0) {
        srand(time(NULL));
        for (int i = 0; i < SIZE; i++) {
            arr[i] = rand() % 21;
            printf("%f\t", arr[i]);
        }
    }

    int *sendcounts = new int[ProcNum];
    int *displs = new int[ProcNum];

    for (int i = 0; i < ProcNum; i++) {
        sendcounts[i] = SIZE / ProcNum;
        if (rem > 0) {
            sendcounts[i]++;
            rem--;
        }
        q -= sendcounts[i];
        displs[i] = q;
    }
    double rec_buf[sendcounts[ProcRank]];

    if (0 == ProcRank) {
        printf("\n");
        for (int i = 0; i < ProcNum; i++) {
            printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
        }
    }

    MPI_Scatterv(&arr, sendcounts, displs, MPI_DOUBLE, &rec_buf, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    printf("%d: ", ProcRank);
    for (int i = 0; i < sendcounts[ProcRank]; i++) {
        printf("%f\t", rec_buf[i]);
    }
    printf("\n");

    int temp_i = 0;
    double send_buf[sendcounts[ProcRank]];
    for (int i = sendcounts[ProcRank] - 1; i >= 0; i--) {
        send_buf[temp_i] = rec_buf[i];
        temp_i++;
    }

    temp_i = 0;
    rem = SIZE % ProcNum;
    q = 0;
    int *displs_r = new int[ProcNum];
    for (int i = 0; i < ProcNum; i++) {
        if (rem > 0) {
            rem--;
        }
        displs_r[i] = q;
        q += sendcounts[i];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("%d: ", ProcRank);
    for (int i = 0; i < sendcounts[ProcRank]; i++) {
        printf("%f\t", send_buf[i]);
    }
    printf("\n");

    double res[SIZE];
    MPI_Gatherv(&send_buf[0], sendcounts[ProcRank], MPI_DOUBLE, &res[0], sendcounts , displs_r, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (0 == ProcRank) {
        printf("\n");
        for (int i = 0; i < ProcNum; i++) {
            printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs_r[i]);
        }
    }

    if (0 == ProcRank) {
        printf("\n");
        for (int i = 0; i < SIZE; i++) {
            printf("%f\t", res[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
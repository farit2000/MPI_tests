#include <stdio.h>
#include <iostream>
#include "mpi.h"

#define SIZE 4

int main (int argc, char *argv[]) {
    double rec_m_buf[100], rec_v_buf[100];
//    double matrix[SIZE][SIZE], vector[SIZE];
    int ProcNum, ProcRank, rem, sum_v = 0, sum_m = 0;

    double matrix[SIZE][SIZE] = {
            {5.0, 10.0, 6.0, 7.0},
            {5.0, 9.0, 4.0, 5.0},
            {5.0, 7.0, 8.0, 6.0},
            {3.0, 4.0, 6.0, 1.0}
    };
    double vector[SIZE] = {1.0, 2.0, 3.0, 4.0};

    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0) {
        srand(time(NULL));
        for (int i = 0; i < SIZE; i++ )
            vector[i] = rand() % 21;
        for (int i = 0; i < SIZE; i++ )
            for (int j = 0; j < SIZE; j++)
            {
                matrix[i][j] = rand() % 21;
            }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    rem = SIZE % ProcNum;
    int *sendcounts_m = new int[ProcNum];
    int *displs_m = new int[ProcNum];
    int *sendcounts_v = new int[ProcNum];
    int *displs_v = new int[ProcNum];

    for (int i = 0; i < ProcNum; i++) {
        sendcounts_m[i] = (SIZE / ProcNum) * SIZE;
        sendcounts_v[i] = SIZE / ProcNum;
        if (rem > 0) {
            sendcounts_m[i] += SIZE;
            sendcounts_v[i] += 1;
            rem--;
        }
        displs_v[i] = sum_v;
        displs_m[i] = sum_m;
        sum_v += sendcounts_v[i];
        sum_m += sendcounts_m[i];
    }

    if (0 == ProcRank) {
        for (int i = 0; i < ProcNum; i++) {
            printf("sendcounts_m[%d] = %d\tdispls_m[%d] = %d\n", i, sendcounts_m[i], i, displs_m[i]);
        }
        printf("\n");
        for (int i = 0; i < ProcNum; i++) {
            printf("sendcounts_v[%d] = %d\tdispls_v[%d] = %d\n", i, sendcounts_v[i], i, displs_v[i]);
        }
    }

    MPI_Scatterv(&matrix, sendcounts_m, displs_m, MPI_DOUBLE, &rec_m_buf, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&vector, sendcounts_v, displs_v, MPI_DOUBLE, &rec_v_buf, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    printf("%d: ", ProcRank);
    for (int i = 0; i < sendcounts_v[ProcRank]; i++) {
        printf("%f\t", rec_v_buf[i]);
    }
    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);
    printf("%d: ", ProcRank);
    for (int i = 0; i < sendcounts_m[ProcRank]; i++) {
        printf("%f\t", rec_m_buf[i]);
    }
    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);
    int i1 = 0;
    int q = 0;
    for (int i = 0; i < sendcounts_m[ProcRank]; i++) {
        if (q == SIZE)
        {
            i1++;
            q = 0;
        }
        q++;
            rec_m_buf[i] = rec_m_buf[i] * rec_v_buf[i1];
    }
    for (int i = SIZE; i < sendcounts_m[ProcRank]; i++) {
        rec_m_buf[i] += rec_m_buf[i - SIZE];
    }
    int temp_i = 0;
    double arr[SIZE];// новый массив нужной тебе длины
    for (int i = sendcounts_m[ProcRank] - SIZE; i < sendcounts_m[ProcRank]; i++)
    {
        arr[temp_i] = rec_m_buf[i];// копирование
        temp_i ++;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    printf("\n");
    printf("%d: ", ProcRank);
    for (int i = 0; i < SIZE; i++) {
    printf("%f\t", arr[i]);
    }
    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);
    double lastRec[SIZE];
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&arr[0], &lastRec[0], SIZE, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (ProcRank == 0) {
        printf("Result :");
        printf("\n");
        for (int i = 0; i < SIZE; i++) {
            printf("%f\t", lastRec[i]);
        }
    }
    MPI_Finalize();
    return 0;
}
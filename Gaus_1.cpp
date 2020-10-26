#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "mpi.h"

int get_chunk(int total, int commsize, int rank){
    int n = total;
    int q = n / commsize;
    if (n % commsize)
        q++;
    int r = commsize * q -n;
    int chunk = q;
    if (rank >= commsize-r)
        chunk = q -1;
    return chunk;
}

int main(int argc, char *argv[]) {
    int n = 10;
    int rank, commsize;
    MPI_Init(&argc, &argv);
    double t = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    int nrows= get_chunk(n, commsize, rank);
    int *rows = new int[sizeof(*rows) * nrows];
    double *a = new double[sizeof(*a) * nrows* (n + 1)];
    double *x = new double[sizeof(*x) * n];
    double *tmp= new double[sizeof(*tmp) * (n + 1)];
    for (int i = 0; i < nrows; i++) {
        rows[i] = rank + commsize * i;
        srand(rows[i] * (n + 1));
        for (int j = 0; j < n; j++)
            a[i * (n + 1) + j] = rand() % 100 + 1;
            a[i * (n + 1) + n] = rand() % 100 + 1;
    }
    MPI_Recv(NULL, 0, MPI_INT, (rank > 0) ? rank -1 : MPI_PROC_NULL, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Proc %d: ", rank);
    for (int i = 0; i < nrows; i++)
        printf("%d ", rows[i]);
    printf("\n");
    MPI_Ssend(NULL, 0, MPI_INT, rank != commsize-1 ? rank + 1 : MPI_PROC_NULL, 0, MPI_COMM_WORLD);
    // Прямой ход
    int row = 0;
    for (int i = 0; i < n -1; i++) {
        if (i == rows[row]) {
            MPI_Bcast(&a[row * (n + 1)], n + 1, MPI_DOUBLE, rank, MPI_COMM_WORLD);
            for (int j = 0; j <= n; j++)
                tmp[j] = a[row * (n + 1) + j];
            row++;
        } else {
            MPI_Bcast(tmp, n + 1, MPI_DOUBLE, i % commsize, MPI_COMM_WORLD);
        }
        for (int j = row; j < nrows; j++) {
            double scaling = a[j * (n + 1) + i] / tmp[i];
            for (int k = i; k < n + 1; k++)a[j * (n + 1) + k] -= scaling * tmp[k];
        }
    }
    row = 0;
    for (int i = 0; i < n; i++) {
        x[i] = 0;
        if (i== rows[row]) {
            x[i] = a[row * (n + 1) + n];
            row++;
        }
    }

    if (rank == 0)
        for (int i = 0; i < n; i++)
            printf("%f \t", x[i]);

    // Обратный ход
    row = nrows-1;
    for (int i = n -1; i > 0; i--) {
        if (row >= 0) {
            if (i== rows[row]) {
                x[i] /= a[row * (n + 1) + i];
                MPI_Bcast(&x[i], 1, MPI_DOUBLE, rank, MPI_COMM_WORLD);
                row--;
            } else
                MPI_Bcast(&x[i], 1, MPI_DOUBLE, i% commsize, MPI_COMM_WORLD);
        } else
            MPI_Bcast(&x[i], 1, MPI_DOUBLE, i% commsize, MPI_COMM_WORLD);
        for (int j = 0; j <= row; j++)
            x[rows[j]] -= a[j * (n + 1) + i] * x[i];
    }
    if (rank == 0)
        x[0] /= a[row * (n + 1)];
    MPI_Bcast(x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    free(tmp);
    free(rows);
    free(a);
    t = MPI_Wtime() -t;
    if (rank == 0) {
        printf("\n n %d, procs %d, time (sec) %.6f\n", n, commsize, t);
        printf("MPI X[%d]: ", n);
        for (int i = 0; i < n; i++)
            printf("%f ", x[i]);
        printf("\n");
    }
    free(x);
    MPI_Finalize();
    return 0;
}
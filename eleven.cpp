#include <stdio.h>
#include "mpi.h"
int main (int argc, char *argv[]) {
    int ProcNum, ProcRank;
    int message = 0;
    MPI_Status Status;
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0) {
        message ++;
        MPI_Send(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&message, 1, MPI_INT, ProcNum - 1, ProcNum, MPI_COMM_WORLD, &Status);
        printf("%d", message);
    }

    if (ProcRank > 0 && ProcRank < ProcNum - 1) {
        MPI_Recv(&message, 1, MPI_INT, ProcRank - 1, ProcRank - 1, MPI_COMM_WORLD, &Status);
        message ++;
        MPI_Send(&message, 1, MPI_INT, ProcRank + 1, ProcRank, MPI_COMM_WORLD);
    }

    if (ProcRank == ProcNum - 1) {
        MPI_Recv(&message, 1, MPI_INT, ProcRank - 1, ProcRank - 1, MPI_COMM_WORLD, &Status);
        message++;
        MPI_Send(&message, 1, MPI_INT, 0, ProcNum, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
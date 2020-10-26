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
        printf("\n");
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

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    message = 0;
    int n = 5;
    const int ranks[5] = {1, 2, 3, 4, 5};

    MPI_Group prime_group;
    MPI_Group_incl(world_group, 5, ranks, &prime_group);

    MPI_Comm small_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &small_comm);
    int small_rank = -1, small_size = -1;
    if (MPI_COMM_NULL != small_comm) {
        MPI_Comm_rank(small_comm, &small_rank);
        MPI_Comm_size(small_comm, &small_size);
    }
//
//    MPI_Comm small_comm;
//    MPI_Comm_split(MPI_COMM_WORLD, 5, ProcRank, &small_comm);

//    MPI_Comm_size ( small_comm, &ProcNum);
//    MPI_Comm_rank ( small_comm, &ProcRank);

    if (small_rank == 0) {
        message ++;
        MPI_Send(&message, 1, MPI_INT, 1, 0, small_comm);
        MPI_Recv(&message, 1, MPI_INT, small_size - 1, small_size, small_comm, &Status);
        printf("%d", message);
    }

    if (small_rank> 0 && small_rank < small_size - 1) {
        MPI_Recv(&message, 1, MPI_INT, small_rank - 1, small_rank - 1, small_comm, &Status);
        message ++;
        MPI_Send(&message, 1, MPI_INT, small_rank + 1, small_rank, small_comm);
    }

    if (small_rank == small_size - 1) {
        MPI_Recv(&message, 1, MPI_INT, small_rank - 1, small_rank - 1, small_comm, &Status);
        message++;
        MPI_Send(&message, 1, MPI_INT, 0, small_size, small_comm);
    }

    MPI_Finalize();
    return 0;
}
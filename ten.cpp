#include <stdio.h>
#include "mpi.h"
#include <chrono>
#include <iostream>

#define SIZE 1000

class timer
{
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t> m_beg;

public:
    timer() : m_beg(clock_t::now())
    {
    }

    void reset()
    {
        m_beg = clock_t::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};

int main (int argc, char *argv[]) {
    int ProcNum, ProcRank;

    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank ( MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0) {
        timer t;
        srand(time(NULL));
        double arr[SIZE];
        for (int i = 0; i < SIZE; i++) {
            arr[i] = rand() % 21;
        }
        MPI_Send(&arr[0], SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&arr, SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Send is : %f\n", t.elapsed());
        t.reset();
        MPI_Ssend(&arr[0], SIZE, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(&arr, SIZE, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Ssend is : %f\n", t.elapsed());
        t.reset();
        int bufsize = 8 * SIZE + MPI_BSEND_OVERHEAD;
        double b_arr[bufsize];
        MPI_Buffer_attach(b_arr, bufsize);
        MPI_Bsend(&arr, SIZE, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD);
        MPI_Buffer_detach(&arr, &bufsize);
        MPI_Recv(&arr, SIZE, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Bsend is : %f\n", t.elapsed());
        t.reset();
        MPI_Rsend(&arr[0], SIZE, MPI_DOUBLE, 1, 3, MPI_COMM_WORLD);
        MPI_Recv(&arr, SIZE, MPI_DOUBLE, 1, 3, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Rsend is : %f\n", t.elapsed());
    } else {
        double rec_buf[SIZE];
        MPI_Recv(&rec_buf, SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Send(&rec_buf[0], SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&rec_buf, SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Ssend(&rec_buf[0], SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        MPI_Recv(&rec_buf, SIZE, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        int bufsize = 8 * SIZE + MPI_BSEND_OVERHEAD;
        double b_arr[bufsize];
        MPI_Buffer_attach(b_arr, bufsize);
        MPI_Bsend(&rec_buf[0], SIZE, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
        MPI_Recv(&rec_buf, SIZE, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Rsend(&rec_buf[0], SIZE, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include "mpi.h"
#include <iostream>
using namespace std;

int main (int argc, char *argv[])
{
    int id, np;
    double x, y;
    long int lhit, hit=0,lN, N=1e9;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    lhit = 0;
    srand((unsigned)(time(0)));
    lN = N/np;

    int k = N / np;
    int i1 = k * id;
    int i2 = k * ( id + 1 );
    if ( id == np - 1 ) i2 = N;

    for(int i = i1; i < i2; i++) {
        x = ((double)rand())/((double)RAND_MAX);
        y = ((double)rand())/((double)RAND_MAX);
        if (((x*x) + (y*y)) <= 1) lhit++;
    }
    MPI_Reduce(&lhit,&hit,1,MPI_DOUBLE, MPI_SUM,0,MPI_COMM_WORLD);

    double est;
    est = ((double)hit*4)/(double)N;

    if (id == 0)
    {
        cout << "Количество капель: " << N << endl;
        cout << "Оценка Pi: " << est << endl;
    }
        MPI_Finalize();
        return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    long long elementsPerProcess = (100 * 100 * 100) / numprocs;
    long long *localA = malloc(sizeof(long long) * elementsPerProcess);
    long long *localB = malloc(sizeof(long long) * elementsPerProcess);
    long long localSum = 0;
    long long globalSum = 0;
    double startTime, endTime;

    srand(time(NULL) + rank);

    long long(*a)[100][100] = NULL;
    long long(*b)[100][100] = NULL;

    if (rank == 0)
    {
        a = malloc(sizeof(long long[100][100][100]));
        b = malloc(sizeof(long long[100][100][100]));

        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                for (int k = 0; k < 100; k++)
                {
                    a[i][j][k] = rand() % 42;
                    b[i][j][k] = rand() % 254;
                }
            }
        }
    }

    // record time taken to add all the elements in the arrays
    startTime = MPI_Wtime();

    MPI_Scatter(a, elementsPerProcess, MPI_LONG_LONG_INT, localA, elementsPerProcess, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, elementsPerProcess, MPI_LONG_LONG_INT, localB, elementsPerProcess, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < elementsPerProcess; i++)
    {
        localSum += localA[i] + localB[i];
    }

    MPI_Reduce(&localSum, &globalSum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        endTime = MPI_Wtime();
        printf("It took %f seconds\n", endTime - startTime);
        printf("with %d processes\n", numprocs);
        free(a);
        free(b);
    }

    free(localA);
    free(localB);

    MPI_Finalize();
    return 0;
}

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
    int *localA = malloc(sizeof(int) * elementsPerProcess);
    int *localB = malloc(sizeof(int) * elementsPerProcess);
    int *localC = malloc(sizeof(int) * elementsPerProcess);
    double startTime, endTime;

    srand(time(NULL) + rank);

    int(*a)[100][100] = NULL;
    int(*b)[100][100] = NULL;
    int(*c)[100][100] = NULL;

    if (rank == 0)
    {
        a = malloc(sizeof(int[100][100][100]));
        b = malloc(sizeof(int[100][100][100]));
        c = malloc(sizeof(int[100][100][100]));

        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                for (int k = 0; k < 100; k++)
                {
                    a[i][j][k] = rand() % 100;
                    b[i][j][k] = rand() % 100;
                }
            }
        }
    }

    // record time taken to add all the elements in the arrays
    startTime = MPI_Wtime();

    MPI_Scatter(a, elementsPerProcess, MPI_INT, localA, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, elementsPerProcess, MPI_INT, localB, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < elementsPerProcess; i++)
    {
        localC[i] = localA[i] + localB[i];
    }

    MPI_Gather(localC, elementsPerProcess, MPI_INT, c, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        endTime = MPI_Wtime();
        printf("It took %f seconds\n", endTime - startTime);
        printf("with %`d processes\n", numprocs);
        free(a);
        free(b);
        free(c);
    }

    free(localA);
    free(localB);
    free(localC);

    MPI_Finalize();
    return 0;
}

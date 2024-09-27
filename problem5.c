#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>

void calculateTimeTaken(struct timeval start, struct timeval end, const char *messageType, int rank, int messageSize)
{
    long tSend = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_usec - start.tv_usec) * 1000;
    printf("Rank =%d; Time taken %s message of size %d: %ld nanoseconds\n", rank, messageType, messageSize, tSend);
}

void calculateLatency(int rank, int dest)
{
    struct timeval start, end;

    if (rank == 1)
    {
        int x = 1;
        gettimeofday(&start, NULL);
        MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, "Latency sending", rank, sizeof(x));
    }
    else if (rank == 0)
    {
        int y = 0;
        MPI_Status status;
        gettimeofday(&start, NULL);
        MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, "Latency receiving", rank, sizeof(y));
    }
}

void calculateBandwidth(int rank, int dest, int messageSize)
{
    struct timeval start, end;
    char *message = malloc(messageSize * sizeof(char));
    for (int i = 0; i < messageSize; i++)
    {
        message[i] = 'a';
    }
    if (rank == 1)
    {
        gettimeofday(&start, NULL);
        MPI_Send(message, messageSize, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, "Bandwith sending", rank, messageSize);
    }
    else if (rank == 0)
    {
        MPI_Status status;
        gettimeofday(&start, NULL);
        MPI_Recv(message, messageSize, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, "Bandwidth receiving", rank, messageSize);
    }

    free(message);
}

int main(int argc, char *argv[])
{
    int rank, p;
    struct timeval start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    assert(p >= 2);
    int messageSizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

    calculateLatency(rank, 1 - rank);
    for (int i = 0; i < 11; i++)
    {
        calculateBandwidth(rank, 1 - rank, messageSizes[i]);
    }

    MPI_Finalize();
    return 0;
}
#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>

void calculateTimeTaken(struct timeval start, struct timeval end, int rank, int messageSize, FILE *file)
{
    long tSend = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_usec - start.tv_usec) * 1000;
    printf("Rank = %d; Time taken for message of size %d: %ld nanoseconds\n", rank, messageSize, tSend);

    // Write only the numerical value to the file in CSV format
    if (file != NULL)
    {
        fprintf(file, "%d,%d,%ld\n", rank, messageSize, tSend);
    }
}

void calculateLatency(int rank, int dest, FILE *file)
{
    struct timeval start, end;

    if (rank == 1)
    {
        int x = 1;
        gettimeofday(&start, NULL);
        MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, rank, sizeof(x), file);
    }
    else if (rank == 0)
    {
        int y = 0;
        MPI_Status status;
        gettimeofday(&start, NULL);
        MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, rank, sizeof(y), file);
    }
}

void calculateBandwidth(int rank, int dest, int messageSize, FILE *file)
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
        calculateTimeTaken(start, end, rank, messageSize, file);
    }
    else if (rank == 0)
    {
        MPI_Status status;
        gettimeofday(&start, NULL);
        MPI_Recv(message, messageSize, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        calculateTimeTaken(start, end, rank, messageSize, file);
    }

    free(message);
}

int main(int argc, char *argv[])
{
    int rank, p;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    assert(p >= 2);

    // Open files for writing in CSV format
    FILE *latencyFile = fopen("mpi_latency_results.csv", "w");
    FILE *bandwidthFile = fopen("mpi_bandwidth_results.csv", "w");

    // Write headers to the CSV files
    fprintf(latencyFile, "Rank,MessageSize,TimeTaken\n");
    fprintf(bandwidthFile, "Rank,MessageSize,TimeTaken\n");

    int messageSizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

    // Calculate Latency
    calculateLatency(rank, 1 - rank, latencyFile);

    // Calculate Bandwidth
    for (int i = 0; i < 11; i++)
    {
        calculateBandwidth(rank, 1 - rank, messageSizes[i], bandwidthFile);
    }

    // Close the files after all operations are done
    fclose(latencyFile);
    fclose(bandwidthFile);

    MPI_Finalize();
    return 0;
}

/* Cpt S 411, Introduction to Parallel Computing
* School of Electrical Engineering and Computer Science
*
* Example code
* Send receive test:
* rank 1 sends to rank 0 (all other ranks sit idle)
* For timing use of C gettimeofday() is recommended.
this is a draft funcion for timing
unsigned long utime(void)
{
struct timeval tv;
unsigned long result = 0;
gettimeofday(&tv, NULL);
result += (tv.tv_sec * 1000000);
result += tv.tv_usec;
return result;
}
* */
#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
int main(int argc, char *argv[])
{
    int rank, p;
    struct timeval start, end;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    printf("my rank=%d\n", rank);
    printf("Rank=%d: number of processes =%d\n", rank, p);
    assert(p >= 2);
    if (rank == 1)
    {
        int x = 10, y = 0;
        int dest = 0;
        // sending
        gettimeofday(&start, NULL);
        MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        gettimeofday(&end, NULL);
        double tSend = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        printf("Rank=%d: sent message %d to rank %d; Send time %.2f millisec\n", rank, x, dest, tSend);
        // receiving
        MPI_Status status;
        gettimeofday(&start, NULL);
        MPI_Recv(&y, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        double tRecv = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        printf("Rank=%d: received message %d from rank %d; Recv time %.2f millisec\n", rank, y, status.MPI_SOURCE, tRecv);
    }
    else if (rank == 0)
    {
        int y = 0;
        MPI_Status status;
        // receiving
        gettimeofday(&start, NULL);
        MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        gettimeofday(&end, NULL);
        double tRecv = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        printf("Rank=%d: received message %d from rank %d; Recv time %.2f millisec\n", rank, y, status.MPI_SOURCE, tRecv);
        // sending
        gettimeofday(&start, NULL);
        MPI_Send(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        gettimeofday(&end, NULL);
        double tSend = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        printf("Rank=%d: sent message %d to rank %d; Send time %.2f millisec\n", rank, y, 1, tSend);
    }
    MPI_Finalize();
    return 0;
}
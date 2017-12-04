#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>

void genMatrix(int *A, int N, int M) {
	// Clean matrix
    for (int i=0; i<N*M; i++) {
		A[i] = 0;
	}
    // Generate matrix
    int m = M/4;
    for (int i=0; i<N; i++) {
		for (int j=0; j<m; j++) A[i*M+j] = (int)((double)rand()/RAND_MAX*99) + 1;
		if (i > 0 && (i+1) % (N/4) == 0) m += M/4;
    }  
}

void sortAndFindMedian(int M, int N,int *A,float *medians){
	int t, n;
	for (int k=0; k<N; k++) {
		n = 0;
		while (A[k*M+n] != 0 && n < M) n++;
		//sort row
		for (int i=0; i<n-1; i++) {
			for (int j=0; j<n-1; j++) {
				if (A[k*M+j] > A[k*M+j+1]) {
					t = A[k*M+j];
					A[k*M+j] = A[k*M+j+1];
					A[k*M+j+1] = t;
				}	
			}
		}
		//find row median
		if (n%2 != 0) {
			medians[k]=static_cast<float>(A[k*M +n/2]);
		}
		else {
			medians[k] =(static_cast<float>(A[k*M+(n/2-1)]+A[k*M+(n/2)]))/static_cast<float>(2);
		}
	}
}


int main(int argc, char** argv) {
    srand(time(NULL));
    int N = 256; //eiluciu_sk
    int M = 200; //stulpeliu_sk
	int nBlocks = 4;
	int *globalA = new int[N*M];
    float *globalMedians = new float[N];
	int nProcs, rank;
	double mpi_startTime, mpi_endTime;
	for(int i=0;i<N;i++){
		globalMedians[i]=0;
	}
    genMatrix(globalA, N, M);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Barrier(MPI_COMM_WORLD);
	mpi_startTime = MPI_Wtime();
	int *localA = new int[N*M/nBlocks/nProcs];
	float *localMedians = new float[N/nBlocks/nProcs];
	for(int i = 0; i< nBlocks; i++){
		MPI_Scatter(globalA+i*M*N/nBlocks, N*M/nBlocks/nProcs, MPI_INT, localA, N*M/nBlocks/nProcs, MPI_INT, 0, MPI_COMM_WORLD);
		sortAndFindMedian(M, N/nBlocks/nProcs, localA, localMedians);
		MPI_Gather(localMedians, N/nBlocks/nProcs, MPI_FLOAT, globalMedians+i*N/nBlocks, N/nBlocks/nProcs, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Gather(localA, N*M/nBlocks/nProcs, MPI_INT, globalA+i*N*M/nBlocks, N*M/nBlocks/nProcs, MPI_INT, 0, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	mpi_endTime = MPI_Wtime();
	if (rank == 0) {
		//print matrix
		printf("Array after sorting rows\n");
		for (int i=0; i<N; i++) {
			for (int j=0; j<M; j++) {
				printf("%3d", globalA[i*M+j]);
			}
			printf("\n");
		}
		
		for(int i; i<N; i++){
			printf("Row %d median value is %.1f\n", i, globalMedians[i]);
		}
		
		printf("MPI time: %f\n", mpi_endTime-mpi_startTime);
	}
	MPI_Finalize();
}

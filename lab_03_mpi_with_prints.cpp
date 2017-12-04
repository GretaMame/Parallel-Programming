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
		printf("n: %d\n", n);
		printf("Before sorting values:\n");
		for(int i=0; i<n;i++){
			printf("%d ", A[k*M+i]);
		}
		printf("\n");
		for (int i=0; i<n-1; i++) {
			for (int j=0; j<n-1; j++) {
				if (A[k*M+j] > A[k*M+j+1]) {
					t = A[k*M+j];
					A[k*M+j] = A[k*M+j+1];
					A[k*M+j+1] = t;
				}	
			}
		}
		printf("Sorted values:\n");
		for(int i=0; i<n;i++){
			printf("%d ", A[k*M+i]);
		}
		printf("\n");
		if (n%2 != 0) {
			medians[k]=static_cast<float>(A[k*M +n/2]);
		}
		else {
			medians[k] =(static_cast<float>(A[k*M+(n/2-1)]+A[k*M+(n/2)]))/static_cast<float>(2);
		}
		printf("Row %d median value is %.1f\n", k, medians[k]);
	}
}


int main(int argc, char** argv) {
    srand(time(NULL));
    int N = 16; //eiluciu_sk
    int M = 20; //stulpeliu_sk
	int blockCount = 4;
	int *globalA = new int[N*M];
    float *globalMedians = new float[N];
	for(int i=0;i<N;i++){
		globalMedians[i]=0;
	}
	int nprocs, rank;
    genMatrix(globalA, N, M);
	double mpi_startTime, mpi_endTime;
	//MPI_Wtime(); ??
	//turi procesas priimt bloka surinkiuot apskaiciuot medianas ir nusiust masteriui
	//kiekviena bloka vykdo visi procesai
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	mpi_startTime = MPI_Wtime();
	int *localA = new int[N*M/blockCount/nprocs];
	float *localMedians = new float[N/blockCount/nprocs];
	for(int i = 0; i< blockCount; i++){
		
		MPI_Scatter(globalA+i*M*N/blockCount, N*M/blockCount/nprocs, MPI_INT, localA, N*M/blockCount/nprocs, MPI_INT, 0, MPI_COMM_WORLD);
		printf("Processor %d has data:\n", rank);
		for(int j=0; j<N*M/blockCount/nprocs; j++){
			printf("%d ", localA[j]);
		}
		printf("\n\n");
		sortAndFindMedian(M, N/blockCount/nprocs, localA, localMedians);
		printf("After function call processor %d has data:\n", rank);
		for(int j=0; j<N*M/blockCount/nprocs; j++){
			printf("%d ", localA[j]);
		}
		printf("\n\n");
		MPI_Gather(localMedians, N/blockCount/nprocs, MPI_FLOAT, globalMedians+i*N/blockCount, N/blockCount/nprocs, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Gather(localA, N*M/blockCount/nprocs, MPI_INT, globalA+i*N*M/blockCount, N*M/blockCount/nprocs, MPI_INT, 0, MPI_COMM_WORLD);
	}
	mpi_endTime = MPI_Wtime();
	MPI_Finalize();
	if (rank == 0) {
		//print matrix
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
}

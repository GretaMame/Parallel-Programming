#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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


int main() {
   srand(time(NULL));
   int N = 16;
   int M = 20;
   int *A = new int[N*M];
   int t, n; 
   int medians = new int[N];
   genMatrix(A, N, M);
 
   for (int k=0; k<N; k++) {
      n = 0;
      while (A[k*M+n] != 0 && n < M) n++;
      for (int i=0; i<n-1; i++) {
         for (int j=0; j<n-1; j++) {
            if (A[k*M+j] > A[k*M+j+1]) {
               t = A[k*M+j];
               A[k*M+j] = A[k*M+j+1];
               A[k*M+j+1] = A[k*M+j];
            }
         }
		 //medianos skaiciavimas
		 if (M%2 != 0) {
			 //jei eilutes ilgis nelyginis
			 medians[i] = A[M/2+1];
			 printf("median pos: %d median: %d", M/2+1, medians[i]);
		 }
		 else {
			 medians[i] = (A[M/2]+A[M/2+1])/2;
			 printf("median pos: %d median: %d", M/2+1, medians[i]);
		 }
      }
   }
   // print matrix
   for (int i=0; i<N; i++) {
      for (int j=0; j<M; j++) 
         printf("%3d", A[i*M+j]);
      }
      printf("\n");
   }
}

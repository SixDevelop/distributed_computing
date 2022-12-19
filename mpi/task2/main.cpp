#include <iostream>
#include <stdio.h> 
#include <unistd.h> 
#include <ctime>
#include <mpi.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define MAX_RAND_VAL 100000   
#define VECTOR_SIZE 1000000     
#define ROOT 0                
#define LOCAL_BUF_SIZE 64

int* generateVector(int size) {
    int* vec = new int[size];
    srand(time(NULL));
    for (int i=0; i<size; i++) {
      vec[i] = rand() % MAX_RAND_VAL + 1;
    };
    return vec;
}


int dotProduct(int* a, int* b, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i];
    };
    return result;
}


int dotProductMPI(int* a, int* b, int n) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int* recBuf1 = new int[n / size];
    int* recBuf2 = new int[n / size];
    int resBuf[LOCAL_BUF_SIZE];

    MPI_Scatter(a, n / size, MPI_INT, recBuf1, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, n / size, MPI_INT, recBuf2, n / size, MPI_INT, 0, MPI_COMM_WORLD);

    int res = dotProduct(recBuf1, recBuf2, n / size);

    MPI_Gather(&res, 1, MPI_INT, resBuf, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int total = 0;
        for (int i = 0; i < size; i++) {
                total += resBuf[i];
        }
        return total;
    }
}


long testSEQ(int vectorSize) {
    int* a = generateVector(vectorSize);
    int* b = generateVector(vectorSize);
    auto start = high_resolution_clock::now();
    dotProduct(a, b, vectorSize);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}


double testMPI(int vectorSize) {
    int rank;
    int* a = generateVector(vectorSize);
    int* b = generateVector(vectorSize);
    double start, end;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == ROOT){
        start = MPI_Wtime();
    }
    int result = dotProductMPI(a, b, vectorSize);
    if(rank == ROOT){
        end = MPI_Wtime();
    }
    return end - start;
}


int main(int args, char** argv) {
    MPI_Init(&args, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    long seqAlgExecTime = testSEQ(VECTOR_SIZE);
    double mpiAlgExecTime1 = testMPI(VECTOR_SIZE);
    double mpiAlgExecTime2 = testMPI(VECTOR_SIZE * 2);
    double mpiAlgExecTime3 = testMPI(VECTOR_SIZE * 4);
    double mpiAlgExecTime4 = testMPI(VECTOR_SIZE * 6);

    
    if(rank == 0) {
        cout << "Vector size: " << VECTOR_SIZE << endl;
        cout << "Sequential algorithm execution time: " << seqAlgExecTime << endl;
        cout << "Vector Size: " << VECTOR_SIZE << ", " << "MPI algorithm execution time: " << mpiAlgExecTime1 * 1000 << endl;
        cout << "Vector Size: " << VECTOR_SIZE * 2 << ", " << "MPI algorithm execution time: " << mpiAlgExecTime2 * 1000 << endl;
        cout << "Vector Size: " << VECTOR_SIZE * 4 << ", " << "MPI algorithm execution time: " << mpiAlgExecTime3 * 1000 << endl;
        cout << "Vector Size: " << VECTOR_SIZE * 6 << ", " << "MPI algorithm execution time: " << mpiAlgExecTime4 * 1000 << endl;
    }
    MPI_Finalize();    
    return 0;
}
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <list>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define VECTOR_SIZE 1000
#define MAX_RAND_VAL 10000
#define VEC_PACK_SIZE 4

const char* filename = "vectors";


int* generateVector(int size) {
    int* vec = new int[size];
    for (int i=0; i<size; i++) {
      vec[i] = rand() % MAX_RAND_VAL + 1;
    };
    return vec;
}


void fillFileWithVectors(const char* filename, int numOfVec, int vecSize) {
    FILE* file;
    file = fopen(filename, "w");

    for(int i = 0; i < numOfVec; i++) {
        int* vec = generateVector(vecSize);
        for (int j = 0; j < vecSize; j++) {
			fprintf(file, "%d\t", vec[j]);
		}
		fprintf(file, "\n");
    }
    fclose(file);
    cout << "Vectors have generated" << endl;
}


void read2VectorsFromFile(FILE* file, int* vec1, int* vec2, int vecSize) {
    for (int i = 0; i < vecSize; i++) {
		fscanf(file, "%d\t", &vec1[i]);
	}
	fscanf(file, "\n");
    for (int i = 0; i < vecSize; i++) {
		fscanf(file, "%d\t", &vec2[i]);
	}
	fscanf(file, "\n");
}


int dotProduct(int* a, int* b, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += a[i] * b[i];
    };
    return result;
}


long testSequential(FILE* file, int numOfVectors, int vecSize) {
    long total = 0;
    int* vec1 = new int[vecSize];
    int* vec2 = new int[vecSize];
    for(int i = 0; i < numOfVectors / 2; i++) {
        read2VectorsFromFile(file, vec1, vec2, vecSize);
        total += dotProduct(vec1, vec2, vecSize);
    }
    return total;
}


long testOMP(FILE* file, int numOfVectors, int vecSize) {
    long total = 0;
    bool flag1, flag2;
    int buffer[VEC_PACK_SIZE][vecSize];

    for(int i = 0; i < numOfVectors / VEC_PACK_SIZE; i++) {
        flag1 = 0;
        flag2 = 0;
        #pragma omp sections
        {
            #pragma omp section
            {
                read2VectorsFromFile(file, buffer[0], buffer[1], vecSize);
                flag1 = 1;
                read2VectorsFromFile(file, buffer[2], buffer[3], vecSize);                
                flag2 = 1;
            }
            #pragma omp section
            {
                if(flag1) {
                    total += dotProduct(buffer[0], buffer[1], vecSize);
                }
                if(flag2) {
                    total += dotProduct(buffer[2], buffer[3], vecSize);
                }
            }
        }
    }
    return total;
}


int test(int numvOfVectors, int vecSize) {
    fillFileWithVectors(filename, numvOfVectors, vecSize);
    FILE* file = fopen(filename, "r");
    auto start = high_resolution_clock::now();
    long seqResult = testSequential(file, numvOfVectors, vecSize);
    auto end = high_resolution_clock::now();
    cout << "Sequential algorithm execution time: " << duration_cast<milliseconds>(end - start).count() << endl;
    fclose(file);

    file = fopen(filename, "r");
    double pStart = omp_get_wtime();
    long ompResult = testOMP(file, numvOfVectors, vecSize);
    double pEnd = omp_get_wtime();
    cout << "Parallel algorithm execution time: " << (pEnd - pStart) << endl;
    fclose(file);

    if(seqResult != ompResult) {
        cout << "Test results do not match" << endl;
        return -1;
    }
    else {
        cout << "Tests completed correctly" << endl;
        return 0;
    }
}


int main() {
    test(1600, 10);
}
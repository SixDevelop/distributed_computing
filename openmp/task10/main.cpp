#include <iostream>
#include <math.h> 
#include </usr/local/Cellar/libomp/15.0.6/include/omp.h>
#include <stdio.h>
#include <list>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define MAX_MATRIX_SIZE 10000   
#define MIN_MATRIX_SIZE 100     
#define ITER_STEP 100           
#define MAX_RAND_VAL 1000000    

#define MAX_THREADS 30          
#define MIN_THREADS 4           
#define THREAD_STEP 2           
#define CORES_NUM 6

#define RETRIES 10


struct TestResult {
    TestResult(int p, double execTime) {
        parameter = p;
        executionTime = execTime;    
    };

    int parameter;
    double executionTime;
};

void saveTestResultsToFile(list<TestResult> testRestults, string filename) {
    ofstream file;
    file.open(filename);
    for (TestResult res : testRestults) {
      file << res.parameter << " " << res.executionTime<< endl;
    }
    file.close();
}


int findMinVectorElement(int* vec, int n)
{
    int min = vec[0];
    for (int i = 1; i < n; i++) {
        if (min >= vec[i])
            min = vec[i];
    }
    return min;
}


int findMinVectorElementOMP(int* vec, int n) {
    int min = vec[0];
    #pragma omp parallel for reduction(min:min)
    for (int i = 1; i < n; i++) {
        if (min >= vec[i])
            min = vec[i];
    }
    return min; 
}


int** generateMatrix(int n, int m) {
    int** matrix = new int* [n];
    srand(time(NULL));
    for (int i=0; i<n; i++) {
        matrix[i] = new int[m];
        for (int j = 0; j < m; j++){
            matrix[i][j] = rand() % MAX_RAND_VAL + 1;
        }
    };
    return matrix;
}

int findMaxMinValue(int** matrix, int n, int m) {
    int maxMin = INT32_MIN;
    for (int i = 0; i < n; i++) {
        int localMin = findMinVectorElement(matrix[i], m);
        if (localMin > maxMin) {
            maxMin = localMin;
        }
    }
    return maxMin;
}

int findMaxMinValueOMP(int** matrix, int n, int m, int threadsNum) {
    int maxMin = INT32_MIN;
    #pragma omp parallel for num_threads(threadsNum)
    for (int i = 0; i < n; i++) {
        int localMin = findMinVectorElement(matrix[i], m);
        #pragma omp critical
        if (localMin > maxMin) {
            maxMin = localMin;
        }
    }
    return maxMin;
}

int findMaxMinValueWithNestedParallelism(int** matrix, int n, int m, int threadsNum) {
    int maxMin = INT32_MIN;
    #pragma omp parallel for num_threads(threadsNum)
    for (int i = 0; i < n; i++) {
        int localMin = findMinVectorElementOMP(matrix[i], m);
        #pragma omp critical
        if (localMin > maxMin) {
            maxMin = localMin;
        }
    }
    return maxMin;
}


long testSeq(int n, int m) {
    int** matrix = generateMatrix(n, m);
    auto start = high_resolution_clock::now();
    findMaxMinValue(matrix, n, m);
    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();        
}


list<TestResult> testParallel(int n, int m) {
    int iters = (MAX_THREADS - MIN_THREADS) / THREAD_STEP;
    int curThreadsNum = MIN_THREADS;
    list<TestResult> results;
    for (int i = 0; i <= iters; i++) {
        cout << "Current threads num: " << curThreadsNum << endl;
        int** matrix = generateMatrix(n, m);
        
        double time = 0.0;
        for (int j = 0; j < RETRIES; j++) {
            double start = omp_get_wtime();
            findMaxMinValueOMP(matrix, n, m, curThreadsNum);
            double end = omp_get_wtime();
            time += end - start;
        }
        results.push_back(TestResult(curThreadsNum, (time / RETRIES) * 1000));
        curThreadsNum += THREAD_STEP;
    }
    return results;
}

list<TestResult> testNestedParallelism(int n, int m) {
    int iters = (MAX_THREADS - MIN_THREADS) / THREAD_STEP;
    int curThreadsNum = MIN_THREADS;
    list<TestResult> results;
    for (int i = 0; i <= iters; i++) {
        cout << "Current threads num: " << curThreadsNum << endl;
        int** matrix = generateMatrix(n, m);
        
        double time = 0.0;
        for (int j = 0; j < RETRIES; j++) {
            double start = omp_get_wtime();
            findMaxMinValueOMP(matrix, n, m, curThreadsNum);
            double end = omp_get_wtime();
            time += end - start;
        }
        results.push_back(TestResult(curThreadsNum, (time / RETRIES) * 1000));
        curThreadsNum += THREAD_STEP;
    }
    return results;
}

int main()
{
    cout << "Sequential algorithm time execution: " << testSeq(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE) << endl;
    list<TestResult> tres1 = testParallel(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    saveTestResultsToFile(tres1, "thread_test.txt");

    list<TestResult> tres2 = testNestedParallelism(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    saveTestResultsToFile(tres2, "nested_parallelism_thread_test.txt");
}
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <list>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define MAX_RAND_VAL 100000    

#define MAX_THREADS 30         
#define MIN_THREADS 4          
#define THREAD_STEP 2          
#define CORES_NUM 10           

#define MAX_ARRAY_SIZE 1000000 
#define MIN_ARRAY_SIZE 10000   
#define ITER_STEP 20000        

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


int* generateVector(int size) {
    int* vec = new int[size];
    srand(time(NULL));
    for (int i=0; i<size; i++) {
      vec[i] = rand() % MAX_RAND_VAL + 1;
    };
    return vec;
}


int findMin(int* vec, int size) {
    int min = INT32_MAX;
    for (int i = 0; i < size; i++) {
        if (min >= vec[i])
            min = vec[i];
    }
    return min;
}


int findMinOMP(int* vec, int size, int threadsNum) {
    int min = INT32_MAX;
    #pragma omp parallel for reduction(min:min) num_threads(threadsNum)
    for (int i = 0; i < size; i++) {
        if (min >= vec[i])
            min = vec[i];
    }
    return min;
}


long testPerfDependsOnTheNumOfThreads(int vectorSize) {
    int* vec = generateVector(vectorSize);
    auto start = high_resolution_clock::now();
    findMin(vec, vectorSize);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();        
}


list<TestResult> testPerfDependsOnTheNumOfThreadsOMP(int vectorSize) {
    int iters = (MAX_THREADS - MIN_THREADS) / THREAD_STEP;
    int curThreadsNum = MIN_THREADS;
    list<TestResult> results;
    for (int i = 0; i <= iters; i++) {
        cout << "Current threads num: " << curThreadsNum << endl;
        int* vec = generateVector(vectorSize);
        
        double time = 0.0;
        for (int j = 0; j < RETRIES; j++) {
            double start = omp_get_wtime();
            findMinOMP(vec, vectorSize, curThreadsNum);
            double end = omp_get_wtime();
            time += end - start;
        }
        results.push_back(TestResult(curThreadsNum, (time / RETRIES) * 1000000));
        curThreadsNum += THREAD_STEP;
    }
    return results;
}

int main(int argc, char *argv[]) {
    cout << "Sequential algorithm time execution: " << testPerfDependsOnTheNumOfThreads(MAX_ARRAY_SIZE) << endl;
    list<TestResult> tres1 = testPerfDependsOnTheNumOfThreadsOMP(MAX_ARRAY_SIZE);
    saveTestResultsToFile(tres1, "thread_test.txt");
}
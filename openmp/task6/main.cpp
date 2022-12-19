#include <omp.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
#include <list>
using namespace std;

#define MAX_THREADS 30         
#define MIN_THREADS 4          
#define THREAD_STEP 2          
#define CORES_NUM 6

#define ITERATION_COUNT 1000000
#define CHUNK_SIZE 10000

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


TestResult runReduction(int threadsNum) {
    int sum = 0;
    double startTime = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) reduction(+: sum) num_threads(threadsNum)
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        sum += 1;
    }
    double endTime = omp_get_wtime();
    return TestResult(threadsNum, endTime - startTime);
}

TestResult runReductionWithAtomic(int threadsNum) {
    int sum = 0;
    double startTime = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) reduction(+: sum) num_threads(threadsNum)
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        #pragma omp atomic
        sum += 1;
    }
    double endTime = omp_get_wtime();
    return TestResult(threadsNum, endTime - startTime);
}

TestResult runReductionWithCritical(int threadsNum) {
    double sum = 0;

    double startTime = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) reduction(+: sum) num_threads(threadsNum)
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        #pragma omp critical
        sum += 1;
    }
    double endTime = omp_get_wtime();
    return TestResult(threadsNum, endTime - startTime);
}

TestResult runReductionWithLock(int threadsNum) {
    double sum = 0;
    omp_lock_t lock;
    omp_init_lock(&lock);
    double startTime = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) reduction(+: sum) num_threads(threadsNum)
    for (int i = 0; i < ITERATION_COUNT; ++i) {
        omp_set_lock(&lock);
        sum += 1;
        omp_unset_lock(&lock);
    }
    double endTime = omp_get_wtime();
    return TestResult(threadsNum, endTime - startTime);
}

list<TestResult> test(TestResult(*func)(int)) {
    int iters = (MAX_THREADS - MIN_THREADS) / THREAD_STEP;
    int curThreadsNum = MIN_THREADS;
    list<TestResult> results;

    for (int i = 0; i <= iters; i++) {
        double start = omp_get_wtime();
        func(curThreadsNum);
        double end = omp_get_wtime();

        results.push_back(TestResult(curThreadsNum, (end - start) * 1000000));

        curThreadsNum += THREAD_STEP;
    }
    return results;
}

int main(){
    list<TestResult> reductionRes = test(runReduction);
    list<TestResult> atomicRes = test(runReductionWithAtomic);
    list<TestResult> criticalRes = test(runReductionWithCritical);
    list<TestResult> lockRes = test(runReductionWithLock);
    
    saveTestResultsToFile(reductionRes, "reduction.txt");
    saveTestResultsToFile(atomicRes, "atomic.txt");
    saveTestResultsToFile(criticalRes, "critical.txt");
    saveTestResultsToFile(lockRes, "lock.txt");
}
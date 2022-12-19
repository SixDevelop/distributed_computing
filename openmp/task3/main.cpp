#include <iostream>
#include <omp.h>
#include <math.h>
#include <list>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

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

double f(double x) {
    return (x * x + 2 * x) * sin(2 * x);
}

double integrate(double(*f)(double), double a, double b, int n) {
    double dx = (b - a) / n;
    double res = 0.0;
    double x = a;
    for (int i = 0; i < n; i++) {
        res += f(x) * dx;
        x += dx;
    }
    return res;
}

double parallelizedInregrate(double(*f)(double), double a, double b, int n, int threadsNum) {
    double dx = (b - a) / n;
    double x = a;
    double sum = 0.0;
    #pragma omp parallel for shared(x,n,dx,f) reduction(+:sum) num_threads(threadsNum)
    for (int i = 0; i < n; i++) {
        sum += f(x + i * dx);
    }
    return sum * dx;
}

long testPerfDependsOnTheNumOfThreads() {
    double a = 3;
    double b = 8;
    int n = 10;
    auto start = high_resolution_clock::now();
    integrate(f, a, b, n);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();        
}


list<TestResult> testPerfDependsOnTheNumOfThreadsOMP() {
    int iters = (MAX_THREADS - MIN_THREADS) / THREAD_STEP;
    int curThreadsNum = MIN_THREADS;
    double a = 3;
    double b = 15;
    int n = 10;

    list<TestResult> results;
    for (int i = 0; i <= iters; i++) {
        cout << "Current threads num: " << curThreadsNum << endl;

        double time = 0.0;
        for (int j = 0; j < RETRIES; j++) {
            double start = omp_get_wtime();
            integrate(f, a, b, n);
            double end = omp_get_wtime();
            time += end - start;
        }
        results.push_back(TestResult(curThreadsNum, (time / RETRIES) * 1000000));
        curThreadsNum += THREAD_STEP;
    }
    return results;
}


int main() {
    cout << "Sequential algorithm time execution: " << testPerfDependsOnTheNumOfThreads() << endl;
    list<TestResult> tres1 = testPerfDependsOnTheNumOfThreadsOMP();
    saveTestResultsToFile(tres1, "thread_test.txt");
}
#include <omp.h>
#include <iostream>
#include "CStopWatch.h"
#include <vector>
#include <unistd.h>
#include <random>

std::random_device rd;                         // only used once to initialise (seed) engine
std::mt19937 rng(rd());                        // random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> uni(0, 2000); // guaranteed unbiased

std::vector<int> generateList(int n) {

    std::vector<int> retValue;
    srandom(time(NULL));
    for(int i = 0; i < n; i++){
      retValue.push_back(uni(rng));
    }

    return retValue;
} 

void countSort(std::vector<int>& a){

    std::vector<int> temp;
    int n, count;

    n = a.size();
    temp.resize(n);

    #pragma omp parallel default(none) private(count) shared(n, a, temp)

    #pragma omp for
    for(int i=0; i<n; i++){
        count = 0;
        for(int j=0; j<n; j++){
            if(a[j] < a[i]){
                count++;
            }else if(a[j] == a[i] && j < i){
                count++;
            }
        }
        temp[count] = a[i];
    }

    a = temp;
}

int main(){

    int threadMin, threadMax, threadStep;
    int numTrials, n;
    CStopWatch timer;
    std::vector<int> result;

    threadMin = 1; threadMax = 12; threadStep = 1;
    numTrials = 1;
    n = 50000;

    for(int numThreads=threadMin; numThreads<=threadMax; numThreads+=threadStep){
        for(int curTrial=0; curTrial<numTrials; curTrial++){
            
            omp_set_num_threads(numThreads);
            result = generateList(n);
            timer.startTimer();
            // Call functions here
            countSort(result);
            timer.stopTimer();
            std::cout << numThreads << ", " << timer.getElapsedTime() << "\n";
            result.clear();
        }
    }
    
    return 0;
}

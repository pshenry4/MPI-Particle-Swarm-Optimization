#include <omp.h>
#include <iostream>
#include "CStopWatch.h"

void serialCountSort(std::vector<int>& a){

    std::vector<int> temp;
    int n;

    n = a.size();
    temp.resize(n);
    for(int i=0; i<n; i++){
        count = 0;
        for(int j=0; j<n; j++){
            if(a[j] < a[i]){
                count++;
            }else if(a[j] == a[i] && j < i){
                count++;
            }
        temp[count] = a[i];
    }

    a = temp;
}

int main(){

    int threadMin, threadMax, threadStep;
    int numTrials;
    CStopWatch timer;


    threadMin = 1; threadMax = 10; threadStep = 1;
    numTrials = 10;

    for(int numThreads=threadMin; numThreads<=threadMax; numThreads++){
        for(int curTrial=0; curTrial<numTrials; curTrial++){
            
            omp_set_num_threads(numThreads);
            timer.startTimer();
            // Call functions here
            timer.stopTimer();
            std::cout << numThreads << ", " << /** Add Results Output Here **/ << ", " << timer.getElapsedTime() << "\n";
        }
    }
    
    return 0;
}

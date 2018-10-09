#include <omp.h>
#include <iostream>

double f(double x){
    double retVal = 0.0;

    retVal = x*x;

    return retVal;
}

double serialTrap(double a, double b, double n){

    double h, retValue, x;

    h = (b-a)/n;
    retValue = (f(a) + f(b))/2.0;
    for(int i=1; i<=n-1; i++){
        x = a + i*h;
        retValue += f(x);
    }
    retValue = h*retValue;

    return retValue;
}

double parallelTrap(double a, double b, double n){
    double h, x, retValue;
    int myId = omp_get_thread_num();
    int numThreads = omp_get_num_threads();

    int myN, myA, myB;

    h = (b-a)/n; 
    myN = n/numThreads;  
    myA = a + myId * myN * h; 
    myB = myA + myN * h; 
    retValue = (f(myA) + f(myB))/2.0; 

    for (int i = 1; i < myN; i++) {
        x = myA + i*h;
        retValue += f(x);
    }
    retValue = retValue * h; 

    return retValue;
}

int main(){

    int numThreads = 4, a, b, n;
    double result;
    
    a = 0, b = 16, n = 16;

    result = serialTrap(a, b, n);
    std::cout << "Serial Result: " << result << std::endl;

    result = 0.0;
    omp_set_num_threads(4);

    #pragma omp parallel num_threads(numThreads) 
    {

        //Incorrect First option
        // #pragma omp critical
        // result += parallelTrap(a, b, n);

        double localResult;
        localResult += parallelTrap(a, b, n);

        #pragma omp critical
        result += localResult;
   }


    std::cout << "Parallel Result: " << result << std::endl;

    return 0;
}
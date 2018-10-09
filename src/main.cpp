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

void parallelTrap(double a, double b, int n, double& result){

    double h, x, myRetValue;
    int myId = omp_get_thread_num();
    int numThreads = omp_get_num_threads();

    int myN, myA, myB;

    h = (b-a)/n; 
    myN = n/numThreads;  
    myA = a + myId * myN * h; 
    myB = myA + myN * h; 
    myRetValue = (f(myA) + f(myB))/2.0; 

    for (int i = 1; i < myN; i++) {
        x = myA + i*h;
        myRetValue += f(x);
    }
    myRetValue = myRetValue * h; 

    #pragma omp critical 
    {
        // std::cout << myRetValue << std::endl;
        result += myRetValue; 
    }
}

int main(){

    int numThreads = 4, a, b, n;
    double result;
    
    a = 0, b = 16, n = 16;

    result = serialTrap(a, b, n);
    std::cout << "Serial Result: " << result << std::endl;

    result = 0.0;
    omp_set_num_threads(4);
    #pragma omp parallel
    parallelTrap(a, b, n, result);
    std::cout << "Parallel Result: " << result << std::endl;

    return 0;
}
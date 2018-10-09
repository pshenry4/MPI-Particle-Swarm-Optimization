#include <omp.h>
#include <iostream>

double f(double x){
    double retVal = 0.0;

    retVal = x*x;

    return retVal;
}

double serialTrap(double a, double b, int n){

    double h, retValue;

    h = (b-a)/n;
    retValue = (f(a) + f(b))/2.0;

    #pragma omp parallel for reduction(+: retValue) 
    for(int i=1; i<=n-1; i++){
        retValue += f(a + i*h);
    }
    retValue = h*retValue;

    return retValue;
}

int main(){

    int numThreads = 4, a, b, n;
    double result;
    
    a = 0, b = 16, n = 16;

    result = 0.0;
    numThreads = 1;
    omp_set_num_threads(numThreads);
    result = serialTrap(a, b, n);
    std::cout << "Serial Result: " << result << std::endl;

    result = 0.0;
    numThreads = 4;
    omp_set_num_threads(numThreads);
    result = serialTrap(a, b, n);
    std::cout << "Parallel Result: " << result << std::endl;

    return 0;
}
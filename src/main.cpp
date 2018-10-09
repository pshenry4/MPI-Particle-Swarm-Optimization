#ifdef _OPENMP
    #include <omp.h>
#endif
#include <iostream>

double f(double x){
    double retVal = 0.0;

    return retVal;
}
double trapezoidal(double a, double b, double n){

    double h, approx, x;

    h = (b-a)/n;
    approx = (f(a) - f(b))/2.0;
    for(int i=0; i<n; i++){
        x = a + i*h;
        approx += f(x);
    }
    approx = h*approx;

    return approx;
}

int main(){

    #ifdef _OPENMP
        int myRank = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
    #else
        int myRank = 0;
        int numThreads = 1;
    #endif

    return 0;
}
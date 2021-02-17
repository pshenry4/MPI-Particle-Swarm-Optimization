#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <random>

// #include <omp.h>
#include "CStopWatch.h"

using namespace std;

typedef vector<int>       iArray1D;
typedef vector<double>    ldArray1D;
typedef vector<ldArray1D> ldArray2D;
typedef vector<ldArray2D> ldArray3D;

#define PI_F 3.141592654f

std::random_device rd;                          // only used once to initialise (seed) engine
double randDbl(const double& min, const double& max) {
    static thread_local mt19937* generator = nullptr;
    if (!generator) {
        generator = new mt19937(clock());
    }
    uniform_real_distribution<double> distribution(min, max);
    return distribution(*generator);
}

double Rastrigin(ldArray2D &R, int Nd, int p){ // Rastrigin
    double Z = 0, Xi;

    for (int i = 0; i < Nd; i++){
        Xi = R[p][i];
        Z += (pow(Xi, 2) - 10 * cos(2 * PI_F * Xi) + 10);
    }
    return -Z;
}

void PSO(int Np, int Nd, int Nt, float xMin, float xMax, float vMin, float vMax,
         double (*objFunc)(ldArray2D &, int, int), int &numEvals, string functionName)
{

    vector<vector<double>> R(Np, vector<double>(Nd, 0));
    vector<vector<double>> V(Np, vector<double>(Nd, 0));
    vector<double> M(Np, 0);

    ldArray2D pBestPosition(Np, vector<double>(Nd, -INFINITY));
    ldArray1D pBestValue(Np, -INFINITY);

    ldArray1D gBestPosition(Nd, -INFINITY);
    float gBestValue = -INFINITY;

    int lastStep = Nt, bestTimeStep = 0;

    float C1 = 2.05, C2 = 2.05;
    float w, wMax = 0.9, wMin = 0.1;
    float R1, R2;

    CStopWatch timer, timer1;
    float positionTime = 0, fitnessTime = 0, velocityTime = 0, totalTime = 0;

    numEvals = 0;
    timer1.startTimer();

    // Init Population
    for (int p = 0; p < Np; p++){
        for (int i = 0; i < Nd; i++){
            R[p][i] = randDbl(xMin, xMax);
            V[p][i] = randDbl(vMin, vMax);
        }
    }

    // Evaluate Fitness
    for (int p = 0; p < Np; p++){
        M[p] = objFunc(R, Nd, p);
        numEvals++;
    }

    for (int j = 1; j < Nt; j++){

        //Update Positions
        timer.startTimer();
        for (int p = 0; p < Np; p++){
            for (int i = 0; i < Nd; i++){
                R[p][i] = R[p][i] + V[p][i];

                if (R[p][i] > xMax){ R[p][i] = randDbl(xMin, xMax);}
                if (R[p][i] < xMin){ R[p][i] = randDbl(xMin, xMax);}
            }
        }
        timer.stopTimer();
        positionTime += timer.getElapsedTime();

        // Evaluate Fitness
        timer.startTimer();
        for (int p = 0; p < Np; p++){
            M[p] = objFunc(R, Nd, p);
            numEvals++;
        }

        for (int p = 0; p < Np; p++){
            if (M[p] > gBestValue){
                gBestValue = M[p];
                for (int i = 0; i < Nd; i++){
                    gBestPosition[i] = R[p][i];
                }
                bestTimeStep = j;
            }

            // Local
            if (M[p] > pBestValue[p]){
                pBestValue[p] = M[p];
                for (int i = 0; i < Nd; i++){
                    pBestPosition[p][i] = R[p][i];
                }
            }
        }
        timer.stopTimer();
        fitnessTime += timer.getElapsedTime();

        if (gBestValue >= -0.0001){
            lastStep = j;
            break;
        }

        // Update Velocities
        timer.startTimer();
        w = wMax - ((wMax - wMin) / Nt) * j;
        for (int p = 0; p < Np; p++){
            for (int i = 0; i < Nd; i++){
                R1 = randDbl(0, 1);
                R2 = randDbl(0, 1);

                // Original PSO
                V[p][i] = w * V[p][i] + C1 * R1 * (pBestPosition[p][i] - R[p][i]) + C2 * R2 * (gBestPosition[i] - R[p][i]);
                if (V[p][i] > vMax){ V[p][i] = vMin + randDbl(vMin, vMax);}
                if (V[p][i] < vMin){ V[p][i] = vMin + randDbl(vMin, vMax);}
            }
        }
        timer.stopTimer();
        velocityTime += timer.getElapsedTime();
    } // End Time Steps

    timer1.stopTimer();
    totalTime += timer1.getElapsedTime();

    R.clear();
    V.clear();
    M.clear();
    pBestPosition.clear();
    pBestValue.clear();
    gBestPosition.clear();

    cout << functionName    << " "
         << gBestValue      << " "
         << Np              << " "
         << Nd              << " "
         << lastStep        << " "
         << bestTimeStep    << " "
         << numEvals        << " "
         << positionTime    << " "
         << fitnessTime     << " "
         << velocityTime    << " "
         << totalTime       << endl;
}

void runPSO(double xMin, double xMax, double vMin, double vMax,
            double (*rPtr)(ldArray2D &, int, int), string functionName)
{

    int Np, Nd, Nt, numEvals;
    int NdMin, NdMax, NdStep;
    int NpMin, NpMax, NpStep;

    NdMin  = 100;
    NdMax  = 100;
    NdStep = 10; // EXTRA CREDIT: Change NdMin to 10 and include results in evaluation for Extra Credit
    NpMin  = 50;
    NpMax  = 500;
    NpStep = 50;
    Nt = 3000;
    Nd = 30;

    for (Np = NpMin; Np <= NpMax; Np += NpStep){
        for (Nd = NdMin; Nd <= NdMax; Nd += NdStep){
            for (int curTrial = 0; curTrial < 10; curTrial++){
                PSO(Np, Nd, Nt, xMin, xMax, vMin, vMax, rPtr, numEvals, functionName);
            }
            cout << endl;
        }
    }

    cout << endl;
}
int main(){

    double (*rPtr)(ldArray2D &, int, int) = NULL;
    double xMin, xMax, vMin, vMax;

    cout << "Function, Fitness, Np, Nd, lastStep, bestStep, Evals, Position Time, Fitness Time, Velocity Time, Total Time" << endl;

    rPtr = &Rastrigin;
    xMin = -5.12;
    xMax = 5.12;
    vMin = -1;
    vMax = 1;
    runPSO(xMin, xMax, vMin, vMax, rPtr, "F2");

    rPtr = NULL;

    return 0;
}
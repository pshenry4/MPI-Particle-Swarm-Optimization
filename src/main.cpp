#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <mpi.h>

#include "CStopWatch.h"

using namespace std;

typedef std::vector<int>       iArray1D;
typedef std::vector<double>    ldArray1D;
typedef std::vector<ldArray1D> ldArray2D;
typedef std::vector<ldArray2D> ldArray3D;

#define PI_F 3.141592654f


std::random_device rd;                          // only used once to initialise (seed) engine
double randDbl(const double& min, const double& max) {
    static thread_local mt19937* generator = nullptr;
    if (!generator) {
        generator = new mt19937(10);
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
    // Any Necessary Variables Needed
    float C1 = 1.45, C2 = 1.45;
    float w, wMax = 0.9, wMin = 0.1;
    float R1, R2;
    int lastStep = Nt, bestTimeStep = 0, numProcs, myRank, localPopSize, extraPopulation, myEvals;
    std::vector<int> chunkSize, indexForChunkStart;

    // Defined number of processes and current rank for each process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // USING THE SAME CODE I USED FOR SCATTER AND GATHER REFLECTION 05
    // CODE BELOW WILL INDEX ALL CHUNKS OF POPULATION CORRECTLY SO EACH WILL HAVE VALID START AND END INDICES.
    localPopSize = Np / numProcs;
    extraPopulation = Np %  numProcs;
    // Two for loops below will help partition the chunk sizes and displacements of the indexes of each chunk so each population for each process is accurate
    chunkSize.resize(numProcs), indexForChunkStart.resize(numProcs);
    for (int i = 0; i < numProcs; i++) {
        chunkSize[i] = localPopSize;
        if (i < extraPopulation) {
            chunkSize[i] += 1; 
        }
    }

    // First process population will start at index of 0
    indexForChunkStart[0] = 0; 
    // We need numProcs amount of chunks for numProcs local populations
    for (int i = 1; i < numProcs; i++) {
        indexForChunkStart[i] = indexForChunkStart[i - 1] + chunkSize[i - 1]; // Start after previous process' elements
    }


    // The current position for each particle in the population
    vector<vector<double>> currentPosition(Np, vector<double>(Nd, 0));
    // Current velocity of each particle in each dimension
    vector<vector<double>> currentVelo(Np, vector<double>(Nd, 0));
    // Current Fitness for each particle
    vector<double> currentFitness(Np, 0);
    // Personal Bests --- STILL NEED THESE FOR VELOCITY CALCULATION
    ldArray2D pBestPosition(Np, vector<double>(Nd, -INFINITY));
    ldArray1D pBestValue(Np, -INFINITY);
    // Global Best
    ldArray1D gBestPosition(Nd, -INFINITY);
    float gBestValue = -INFINITY;
    // For Local Only
    float localBestValue = -INFINITY;
    ldArray1D localBestPosition(Nd, -INFINITY);
    // Timers Defined
    CStopWatch timer, timer1;
    float positionTime = 0, fitnessTime = 0, velocityTime = 0, totalTime = 0;
    // Evals Defined
    numEvals = 0;
    myEvals = 0;
    

    // Have only master process time all work 
    if (myRank == 0){
        timer1.startTimer();
    }

    // Init Population based on current processes starting and ending indices based on the conditional statement outcome
    for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){
        for (int i = 0; i < Nd; i++){
            currentPosition[p][i] = randDbl(xMin, xMax);
            currentVelo[p][i] = randDbl(vMin, vMax);
        }
    }

    // Evaluate Fitness based on current processes starting and ending indices based on the conditional statement outcome
    for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){
        currentFitness[p] = objFunc(currentPosition, Nd, p);
        myEvals++;
    }


    for (int j = 1; j < Nt; j++){
        // Have only master process time all work 
        if (myRank == 0) {
            timer.startTimer();
        }
        // Update Positions based on current processes starting and ending indices based on the conditional statement outcome
        for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){ // Particle
            for (int i = 0; i < Nd; i++){ // Dimension
                currentPosition[p][i] = currentPosition[p][i] + currentVelo[p][i]; // Update my position

                // Corrects a particle if it's outside the bounds
                if (currentPosition[p][i] > xMax){ currentPosition[p][i] = randDbl(xMin, xMax);}
                if (currentPosition[p][i] < xMin){ currentPosition[p][i] = randDbl(xMin, xMax);}
            }
        }
        // Have only master process time all work 
        if (myRank == 0){
            timer.stopTimer();
            positionTime += timer.getElapsedTime();        
        }



        // Have only master process time all work 
        if (myRank == 0) {
            timer.startTimer();
        }
        // Evaluate Fitness based on current processes starting and ending indices based on the conditional statement outcome
        for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++) {
            currentFitness[p] = objFunc(currentPosition, Nd, p);
            myEvals++;
        }
        // Have only master process time all work 
        if (myRank == 0){
            timer.stopTimer();
            fitnessTime += timer.getElapsedTime();
        }

        // Global & Personal Bests based on current processes starting and ending indices based on the conditional statement outcome
        for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){
            // We need to still track the personal bests that WONT be shared to other processes to keep velocity calculation accurate
            if (currentFitness[p] > pBestValue[p]){
                pBestValue[p] = currentFitness[p];
                pBestPosition[p] = currentPosition[p]; 
            }
            // Track the local best fitness and position 
            if (currentFitness[p] > localBestValue) {
                localBestValue = currentFitness[p];
                localBestPosition = currentPosition[p];
            }
        }
        
        // We can make an array to hold the best value and the rank to use the MAXLOC operation in reduce
        // Using the array setup like we have done when working with different communicator grids in class
        double localBest[2] = {localBestValue, double(myRank)}; // Have to convert the process rank to double so array will be valid
        double currentReducedGlobal[2] = {0, 0}; // Define a current global best for the reduction to hold the best value and then the rank of the process
        
        // Reduce to find the global best value and rank for each process
        // Using MAXLOC which keeps the highest value AND the rank of the process so we don't have to do another reduce/bcast
        MPI_Allreduce(&localBest, &currentReducedGlobal, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

        // Bcast's the local best position to ALL other processes from the currentReducedGlobal array holding the best rank so that value is broadcasted
        MPI_Bcast(localBestPosition.data(), Nd, MPI_DOUBLE, currentReducedGlobal[1], MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD); // Syncs all processes before checking gBestValue status

        // Now after all values are broadcasted and synced from the barrier, check the actual global best to see if the current gBest is better
        if (currentReducedGlobal[0] > gBestValue) {
            // If the currentRedcuedGlobal value is better, then set the gBestVal and location for all processes since all necessary info is broadcasted
            gBestValue = currentReducedGlobal[0];
            gBestPosition = localBestPosition;
            bestTimeStep = j;
        }
        

        // The rest of this holds true now that the gBestValue is accurate
        if (gBestValue >= -0.0001){
            lastStep = j;
            break;
        }
        MPI_Barrier(MPI_COMM_WORLD);

        // Have only master process time all work 
        if (myRank == 0){
            timer.startTimer();
        }
        // Update Velocities based on current processes starting and ending indices based on the conditional statement outcome
        w = wMax - ((wMax - wMin) / Nt) * j;
        for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){

            for (int i = 0; i < Nd; i++){
                R1 = randDbl(0, 1);
                R2 = randDbl(0, 1);
                currentVelo[p][i] = w * currentVelo[p][i] + C1 * R1 * (pBestPosition[p][i] - currentPosition[p][i]) + C2 * R2 * (gBestPosition[i] - currentPosition[p][i]);
                if (currentVelo[p][i] > vMax){ currentVelo[p][i] = randDbl(vMin, vMax);}
                if (currentVelo[p][i] < vMin){ currentVelo[p][i] = randDbl(vMin, vMax);}
            }
        }

        // Have only master process time all work 
        if (myRank == 0){
            timer.stopTimer();
            velocityTime += timer.getElapsedTime();
        }
        MPI_Barrier(MPI_COMM_WORLD);
    } // End Time Steps
    MPI_Barrier(MPI_COMM_WORLD); // Barrier to make sure all processes are completed with the J-Loop

    // Get the correct number of evals by summing up all of the processes loval values
    MPI_Allreduce(&myEvals, &numEvals, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if (myRank == 0) {
        timer1.stopTimer();
        totalTime += timer1.getElapsedTime();
    }

    currentPosition.clear();
    currentVelo.clear();
    currentFitness.clear();
    pBestPosition.clear();
    pBestValue.clear();
    gBestPosition.clear();

    // Have only master process output the results
    if (myRank == 0){
        cout << numProcs << " "
            << functionName    << " "
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

}

void runPSO(double xMin, double xMax, double vMin, double vMax,
            double (*rPtr)(ldArray2D &, int, int), string functionName)
{

    srand(10);
    // Np = Population size
    // Nd = # of  Dimensions
    // Nt = Number of iterations
    int Np, Nd, Nt, numEvals;
    Np = 100;
    Nt = 10000;
    Nd = 100;
    
    for (int curTrial = 0; curTrial < 10; curTrial++){
        PSO(Np, Nd, Nt, xMin, xMax, vMin, vMax, rPtr, numEvals, functionName);
    }
}
int main(){

    MPI_Init(NULL, NULL); // INIT's MPI
    double (*rPtr)(ldArray2D &, int, int) = NULL;
    double xMin, xMax, vMin, vMax;
    int myRank;

    // Defined current rank for each process
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // Have only master output
    if (myRank == 0){
        cout << "Processes, Function, Fitness, Np, Nd, lastStep, bestStep, Evals, Position Time, Fitness Time, Velocity Time, Total Time" << endl;
    }

    rPtr = &Rastrigin;
    xMin = -5.12;   // Lower Bound
    xMax = 5.12;    // Upper Bound
    vMin = -1;      // Minimum Velocity
    vMax = 1;       // Maximum Velocity

    runPSO(xMin, xMax, vMin, vMax, rPtr, "F2");

    rPtr = NULL;

    MPI_Finalize(); // Finalizes MPI 
    return 0;
}
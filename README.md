### Particle Swarm Optimization Using MPI to Parallelize ###


## Summary ##
My work for this assignment parallelizes the particle swarm optimization algorithm through using the OpenMPI library. This library helps us parallelize our programs by adding and configuring how processes communicate with each other, rather than how threads communicate with each other to execute a program in parallel. After determining a solution regarding the code, I ran 1-12 processes, each with 10 trails to obtain valid data for all runs of the program. This data helped me then plot the performance-tracking metrics that are described later in this README file. 

## Algorithm Overview ##
The particle swarm optimization algorithm at a high-level is an iterative algorithm that looks for one candidate solution point on a plane for all other points to "flock" to. This algorithm imitates the process of bird flocking or fish schooling in terms of a real-life scenario. 

Regarding the algorithm itself, there first needs to be a population that is defined/created. The particle swarm optimization algorithm does this through iterating through each "particle" or entity in the population to get the Rastrigin fitness regarding its particle number and the number of dimensions that are being analyzed. The Rastrigin function is defined as

$$ fitness = \sum_{i=1}^{n} \left(X_i^2) - 10 \cdot \cos(2 \cdot \pi \cdot X_i) + 10\right) $$

where "Xi" is the double integer value of the current position in the population. 

The main work in the particle swarm optimization algorithm comes within one main loop in the code. Within this loop, the population positions are all updated, then the fitness is updated again based on the new updated positions. After the fitness is calculated, to find where the particles needs to move/flock to next, the global best fitness value and position needs to be determined. This value will also determine if the flock has converged to one point or not. This can be seen as when the fitness value is > -0.0001, meaning the particle perfectly fits the desired position in the plane, the algorithm will break and be finished. If the algorithm hasn't converged, then the velocity of each particle will be calculated to help determine the next position update before performing the same steps over again until convergence. 

## Parallelization Methodology ##
Overall, parallelizing the PSO algorithm took a lot more time and thinking when using MPI to parallelize rather than OpenMP. This is because of the difference between processes and threads as processes execute the entire program rather than a specific parallel section in the program.

When dealing with processes to paralellize this algorithm, I first had to determine how I could split up the population accurately between all processes. I did this through dividing the population size by the number of processes and finding the remainder of the population if the population didn't divide evenly. The next step was to properly index each chunk of the population. I couldn't just have each population start from index 0 and go to 25, for example as the index of each particle matters when initializing each population. This means that I needed to keep the indices of each population accurate to the total size. For example, if the population was 100 and I have 2 processes, then the population would be initialized from 0-50 and 50-100, rather than 0-50 and 0-50. I accomplished this through using previous indexing code I had from reflection #05, and well as creating a conditional statement in **EVERY** for-loop so the starting and ending indexes were accurate per-iteration. The conditional statement for each for-loop is defined in the line below:
```
for (int p = indexForChunkStart[myRank]; p < ((myRank + 1 < numProcs) ? indexForChunkStart[myRank + 1] : Np); p++){}
```

After splitting up each population correctly, I had to figure out how to have each process communicate their global best positions and values correctly for each J-Loop iteration. To accomplish this, I first defined local best variables for the value and position, while keeping the personal best values in the provided serial version. This is so each process can find their local best value. After finding these values, I setup an array that held the current local best along with its rank that had to be cast as type double to keep the array correct. I then was able to use the line:

```
MPI_Allreduce(&localBest, &currentReducedGlobal, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
```

The line above is an MPI function that will take the values in the array "localBest" in each process, apply the MPI_MAXLOC function to find the max value along with the process rank that holds that value to then store it in the array "currentReducedGlobal" for **ALL** processes. Now that each process has the current global best value and the matching process rank, I then needed to broadcast the position that corresponds with that best value to all processes. I did this through the line:

```
MPI_Bcast(localBestPosition.data(), Nd, MPI_DOUBLE, currentReducedGlobal[1], MPI_COMM_WORLD);
```

The line above overwrites the values in "localBestPosition" for all processes with the best position value for the rank "currentReducedGlobal" which has the current global best value stored. This broadcast function will now guarantee all processes will have the same best position values. After reducing and broadcasting the best values and positions to all processes, the best values are then compared to the previous global best value to see if it is better or not. If the current best is better than the previous, then those values will be updated for all processes. These MPI parallelization methods are then repeating as-needed until the algorithm converges.

The final MPI method I used was to only have the master process control any of the timers. This is so each process didn't alter the timers to cause any inconsistencies.

The main problems I ran into with this assignment were segmentation faults and process received signal errors. This was because I was trying to access uninitialized memory in different vectors in different processes. This was the main issue I ran into with this assignment as MPI is much more complex when thinking of how to organize the memory of each vector properly, especially with the indices of each vector as their are always "N" copies of each vector you have to handle. This issue was occuring mainly when I shared the positions of the best value to all processes, and those shared positions were out of bounds for other position vectors. Once I determined how to correctly index each chunk start and end, and applied it to each for-loop, this issue was resolved.


## Performance-Tracking Functions that we tracked with our Results ##
- **Speedup** is the ratio defined as $$ S_p=T_serial/T_parallel $$ The results of the speedup graph encapsulating the performance of each trial of each thread increase at a high rate as more proccesses were added. This goes to show how the PSO algorithm is "Embarrasingly" parallel. This speedup graph for MPI processes was overall more consistent compared to using OpenMP threads for gaining a speedup. A serial time of around 16 seconds was able to be decreased into about 1.7 seconds. This gave us a very impressive speedup of around 9.00 which is just around the same speedup gained by parallelizing through OpenMP. This suggests that there is no major performance gain by using one or the other for the particle swarm optimization algorithm.
- **Efficiency** is defined as $$E_p = S_p/p $$ The efficiency values in the graph are slightly decreasing per-trial and then eventually leveled-out as more processes were added when executing particle swarm optimization. The decrease in efficiency isn't drastic, which means that most of the allocated cores and power are being used. However, since it is still decreasing as more threads are added, there is room for the problem size to be increased to have full utilization of the allocated number of cores and "power". The leveling-off of the efficiency also suggests that adding more processes doesn't necessarily decrease the efficiency, but better distributes the problem size accross all number of processes.
- **Karp-Flatt** is defined as $$ e = \frac{1/s - 1/p}{1-1/p} $$ The results for the Karp-Flatt graph increased rapidly from 2 to 3 processes, then started to decrease and slightly oscillate through increasing and decreasing values as more processes were added. The lower the value of "e" tends to mean the code is parallelized in the best and most efficient way. Having this metric mainly decrease shows that there is still room for improvement in-terms of how the code is parallelized. However, there were some cases that ended up performing well in-terms of the Karp-Flatt metric such as running PSO with 2, 10, and 12 processes which generated the best Karp-Flatt values.

## Results ##
All results are available in [CSV](./Results/results.csv) and [Excel](./Results/results.xlsx) format. 

<div style="align:center;">

![2D Speedup](./Results/Speedup1.png "2D Speedup")
![3D Speedup](./Results/Speedup2.png "3D Speedup")

![2D Efficiency](./Results/Efficiency1.png "2D Efficiency")
![3D Efficiency](./Results/Efficiency2.png "3D Efficiency")

![2D Karp-Flatt](./Results/Karp1.png "2D Karp-Flatt")
![3D Karp-Flatt](./Results/Karp2.png "3D Karp-Flatt")
</div>

## Running ##
To compile and run from command line if you are not on windows:
```
cd src
mpicxx main.cpp CStopWatch.cpp
mpiexec --allow-run-as-root --oversubscribe -n X MPI
```
or
```
cd Default && make all
mpiexec --allow-run-as-root --oversubscribe -n X MPI
```
where `X` is the number of nodes

## Running on OSC ##
To run on OSC we can use the slurm script provided by running the command:
```
sbatch jobScript.slurm
```

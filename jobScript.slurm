#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --time=1:00:00
#SBATCH --mail-type=END,FAIL
#SBATCH --account=PCS0288
#SBATCH --job-name=Assignment02

module load openmpi 

#module load intel
cd $SLURM_SUBMIT_DIR/Default
make clean && make MPI


echo "Processes, Function, Fitness, Np, Nd, lastStep, bestStep, Evals, Position Time, Fitness Time, Velocity Time, Total Time" >> ../Results/results.csv
for i in {1..12}
do
    mpiexec -n $i MPI >> ../Results/results.csv
done





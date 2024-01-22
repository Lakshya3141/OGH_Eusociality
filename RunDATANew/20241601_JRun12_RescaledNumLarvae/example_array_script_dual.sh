#!/bin/bash

#### SLURM settings ####
#SBATCH --ntasks=1
#SBATCH --job-name=SecondRound
#SBATCH --output=slurm.%j.out
#SBATCH --time=05:00:00
#SBATCH --mem=11GB
#SBATCH --array=1-27
#SBATCH --partition=regular

module load R/4.2.2-foss-2022b
module load foss/2022b
module load Qt5/5.15.5-GCCcore-11.3.0
module load tbb/2020.3-GCCcore-11.2.0
module load Eigen/3.4.0-GCCcore-12.2.0

g++ -std=c++2a -O2 Haplotype.hpp Random.hpp main.cpp Parameters.hpp Individual.hpp Population.hpp config_parser.h -o myprog
chmod +x myprog

parSet=$SLURM_ARRAY_TASK_ID

mkdir $parSet  
cd $parSet
cp ../myprog .
cp ../create_sim_script_Dual.R .
cp ../create_ini.R .
cp ../analysis_evolution.R .
cp ../analysis_LastOfUs.R .
cp ../analysis_deadInd.R .
Rscript create_sim_script_Dual.R ${parSet}
chmod +x myprog
./myprog config.ini
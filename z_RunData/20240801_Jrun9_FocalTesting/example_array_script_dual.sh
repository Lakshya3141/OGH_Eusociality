#!/bin/bash

#### SLURM settings ####
#SBATCH --ntasks=1
#SBATCH --job-name=SecondRound
#SBATCH --output=slurm.%j.out
#SBATCH --time=03:00:00
#SBATCH --mem=15GB
#SBATCH --array=1-27
#SBATCH --partition=regular

module load R/4.2.2-foss-2022b
module load foss/2022b

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
Rscript analysis_evolution.R
Rscript analysis_LastOfUs.R
Rscript analysis_deadInd.R
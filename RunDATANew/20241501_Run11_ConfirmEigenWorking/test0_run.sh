#!/bin/bash

#### SLURM settings ####
#SBATCH --job-name=FirstSolitary_output
#SBATCH --output=test0.out
#SBATCH --time=05:00:00
#SBATCH --mem=200GB
#SBATCH --partition=regular

module load R/4.2.2-foss-2022b
module load foss/2022b
module load Qt5/5.15.5-GCCcore-11.3.0
module load tbb/2020.3-GCCcore-11.2.0
module load Eigen/3.4.0-GCCcore-12.2.0

Rscript create_ini.R
g++ -std=c++2a -O2 Haplotype.hpp Random.hpp main.cpp Parameters.hpp Individual.hpp Population.hpp config_parser.h -o myprog
chmod +x myprog
./myprog config.ini

# Rscript analysis_evolution.R
# Rscript analysis_LastOfUs.R

#!/bin/bash

#### SLURM settings ####
#SBATCH --job-name=FirstSolitary_output
#SBATCH --output=test0.out
#SBATCH --time=05:00:00
#SBATCH --mem=500MB
#SBATCH --partition=regular

module load R/4.2.2-foss-2022b
module load foss/2022b

Rscript create_ini.R
g++ -std=c++2a -O2 Haplotype.hpp Random.hpp main.cpp Parameters.hpp Individual.hpp Population.hpp config_parser.h -o myprog
chmod +x myprog
./myprog config.ini

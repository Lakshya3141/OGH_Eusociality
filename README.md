# OGH_Eusociality
Codes to simulate individual based model of the ovarian groundplan hypothesis. Check developer branch for latest updates

The general procedure of running simulations and analysis is as follows:
1) Edit create_ini.r and/or create_sim_script.R for required parameter ranges.
2) Running simulations: Use either main.cpp (on your terminal, run create_ini.R first) OR bash_individual.sh OR bash_ParamExplore.sh on cluster; 
3) If you ran main.cpp OR bash_individual.sh, directly run Imaging_individual.R to see results
4) If you did parameter exploration, copy mapping_dual.csv from any of the simulation foders
5) Subsequently run confirm_presence.R to highlight folders with copy-data-error OR extinct populations
6) Edit input mapping name in Imaging_combined.R and set conditions_to_compare to conditions of your choice (from mapping) -> Run Imaging_combined.R
7) Run Imaging_replicate.R with specific folder name to get images for representative replicate

DONE!

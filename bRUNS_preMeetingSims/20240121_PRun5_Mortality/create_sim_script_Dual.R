source("create_ini.R")

# example parameter exploration here:

dSurvForage <- seq(0.95, 0.99, 0.02)
dSurvBrood <- seq(0.95, 0.99, 0.02)
# dDeathTime <- seq(2.0, 10.0, 8.0)
replicates <- c(1:3)

# param_grid <- expand.grid(dbt = dBroodingTime, dft = dForagingTime, ddt = dDeathTime, rp = replicates)
# write.csv(param_grid, "mapping_dual.csv")

param_grid <- expand.grid(dsf = dSurvForage, dsb = dSurvBrood, rp = replicates)
write.csv(param_grid, "mapping_dual.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dSurvForage <- param_grid$dsf[sim_number]
focal_dSurvBrood <- param_grid$dsb[sim_number]
# focal_dDeathTime <- param_grid$ddt[sim_number]

# leave other parameters at default:
# create_config(dBroodingTime = focal_dBroodingTime,dForagingTime = focal_dForagingTime, 
#               dDeathTime = focal_dDeathTime, params_to_record = "dBroodingTime,dForagingTime,dDeathTime")
create_config(dSurvForage = focal_dSurvForage,dSurvBrood = focal_dSurvBrood, 
              params_to_record = "dSurveForage,dSurvBrood")

# done

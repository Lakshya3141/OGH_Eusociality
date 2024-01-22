source("create_ini.R")

# example parameter exploration here:

dForagingTime <- seq(4.0,20.0,8.0)
dForagingTimeSD <- seq(0.5,2.5,1.0)
# dDeathTime <- seq(2.0, 10.0, 8.0)
replicates <- c(1:3)

# param_grid <- expand.grid(dbt = dBroodingTime, dft = dForagingTime, ddt = dDeathTime, rp = replicates)
# write.csv(param_grid, "mapping_dual.csv")

param_grid <- expand.grid(dft = dForagingTime, dftsd = dForagingTimeSD, rp = replicates)
write.csv(param_grid, "mapping_dual.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dForagingTime <- param_grid$dft[sim_number]
focal_dForagingTimeSD <- param_grid$dftsd[sim_number]
# focal_dDeathTime <- param_grid$ddt[sim_number]

# leave other parameters at default:
# create_config(dBroodingTime = focal_dBroodingTime,dForagingTime = focal_dForagingTime, 
#               dDeathTime = focal_dDeathTime, params_to_record = "dBroodingTime,dForagingTime,dDeathTime")
create_config(dForagingTimeSD = focal_dForagingTimeSD,dForagingTime = focal_dForagingTime, 
              params_to_record = "dForagingTimeSD,dForagingTime,dBroodingTime")

# done

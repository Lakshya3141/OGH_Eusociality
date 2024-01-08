source("create_ini.R")

# example parameter exploration here:

dBroodingTime <- seq(4.0,20.0,8.0)
dForagingTime <- seq(4.0,20.0,8.0)
dDeathTime <- seq(2.0, 10.0, 8.0)

param_grid <- expand.grid(dbt = dBroodingTime, dft = dForagingTime, ddt = dDeathTime)
write.csv(param_grid, "mapping_dual.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dBroodingTime <- param_grid$dbt[sim_number]
focal_dForagingTime <- param_grid$dft[sim_number]
focal_dDeathTime <- param_grid$ddt[sim_number]

# leave other parameters at default:
create_config(dBroodingTime = focal_dBroodingTime,dForagingTime = focal_dForagingTime, 
              dDeathTime = focal_dDeathTime, params_to_record = "dBroodingTime,dForagingTime,dDeathTime")

# done

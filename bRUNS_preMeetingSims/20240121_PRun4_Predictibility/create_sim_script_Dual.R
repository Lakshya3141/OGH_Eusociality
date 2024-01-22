source("create_ini.R")

# example parameter exploration here:

dForagingMean <- seq(0.5,1.5,0.5)
dForagingSD <- seq(0.1,0.5,0.2)
# dDeathTime <- seq(2.0, 10.0, 8.0)
replicates <- c(1:3)

# param_grid <- expand.grid(dbt = dBroodingTime, dft = dForagingTime, ddt = dDeathTime, rp = replicates)
# write.csv(param_grid, "mapping_dual.csv")

param_grid <- expand.grid(dfm = dForagingMean, dfsd = dForagingSD, rp = replicates)
write.csv(param_grid, "mapping_dual.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dForagingMean <- param_grid$dfm[sim_number]
focal_dForagingSD <- param_grid$dfsd[sim_number]
# focal_dDeathTime <- param_grid$ddt[sim_number]

# leave other parameters at default:
# create_config(dBroodingTime = focal_dBroodingTime,dForagingTime = focal_dForagingTime, 
#               dDeathTime = focal_dDeathTime, params_to_record = "dBroodingTime,dForagingTime,dDeathTime")
create_config(dForagingMean = focal_dForagingMean,dForagingSD = focal_dForagingSD, 
              params_to_record = "dForagingMean,dForagingSD")

# done

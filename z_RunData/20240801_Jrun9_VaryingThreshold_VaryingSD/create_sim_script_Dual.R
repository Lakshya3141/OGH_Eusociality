source("create_ini.R")

# example parameter exploration here:

dForagingSD <- seq(0.1,0.3,0.1)
dLarvaMatureSize <- seq(0.5,1.5,0.5)
replicates <- c(1:2)

param_grid <- expand.grid(fsd = dForagingSD, flm = dLarvaMatureSize, rp = replicates)
write.csv(param_grid, "mapping_dual.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dForagingSD <- param_grid$fsd[sim_number]
focal_dLarvaMatureSize <- param_grid$flm[sim_number]

# leave other parameters at default:
create_config(dForagingSD = focal_dForagingSD,dLarvaMatureSize = focal_dLarvaMatureSize, 
	      params_to_record = "dForagingSD,dLarvaMatureSize")

# done

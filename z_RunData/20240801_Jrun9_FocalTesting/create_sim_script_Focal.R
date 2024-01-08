source("create_ini.R")

# example parameter exploration here:

dForagingSD <- seq(0.1,0.3,0.1)
replicates <- c(1:3)

param_grid <- expand.grid(fsd = dForagingSD, rp = replicates)
write.csv(param_grid, "mapping_focal.csv")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dForagingSD <- param_grid$fsd[sim_number]

# leave other parameters at default:
create_config(dForagingSD = focal_dForagingSD, 
	      params_to_record = "dForagingSD")

# done

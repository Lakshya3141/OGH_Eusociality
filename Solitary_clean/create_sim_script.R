source("create_ini.R")

# example parameter exploration here:

dForagingSD <- seq(0.1,0.3,0.1)
dForagingMean <- seq(0.5,1.5,0.5)
replicates <- c(1:5)

param_grid <- expand.grid(fsd = dForagingSD, fmn = dForagingMean, rp = replicates)
saveRDS(param_grid, "mapping.rds")

# read in the parameters from the command line:
args = commandArgs(trailingOnly = TRUE)
sim_number = as.numeric(args[[1]])

focal_dForagingSD <- param_grid$fsd[sim_number]
focal_dForagingMean <- param_grid$fmn[sim_number]

# leave other parameters at default:
create_config(dForagingSD = focal_dForagingSD,dForagingMean = focal_dForagingMean, 
	      params_to_record = "dForagingSD,dForagingMean")

# done

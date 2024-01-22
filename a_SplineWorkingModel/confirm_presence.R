library(dplyr)
mapping <- read.csv("mapping_dual.csv")

mapping$evolution <- 0
mapping$lou <- 0
mapping$deadInd <- 0
mapping$condition <- ""

for (i in seq(1:dim(mapping)[1])) {
  deadInd <- list.files(paste0(getwd(),"/",mapping$X[i]), pattern = "_deadIndividuals.csv", full.names = TRUE)
  evol <- list.files(paste0(getwd(),"/",mapping$X[i]), pattern = "_evolution.csv", full.names = TRUE)
  lou <- list.files(paste0(getwd(),"/",mapping$X[i]), pattern = "_LastOfUs.csv", full.names = TRUE)
  
  mapping$evolution[i] = length(deadInd) 
  mapping$lou[i] = length(lou)
  mapping$deadInd[i] = length(evol)
  
  if (!(mapping$evolution[i] | mapping$lou[i] | mapping$deadInd[i])) {
    print(paste0("Check folder ",mapping$X[i]))
    mapping$condition[i] = NA
  }
}

write.csv(mapping, "mapping_condition.csv", row.names = FALSE)
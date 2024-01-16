library(ggplot2)
library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)

directory <- getwd()
files <- list.files(directory, pattern = "_deadIndividuals.csv", full.names = TRUE)

# Loop through each file
for (file in files) {
  # Extract simulation ID from the filename
  simulationID <- as.numeric(str_extract(file, "\\d+"))
  data <- read.csv(file)
  cat("Analysing dead individuals for simulation ID:", simulationID, "\n")
}

dir.create("OutputImages", showWarnings = FALSE)

dpt_pertime <- ggplot(data, aes(x = gtime, y = pertime_foraging)) +
  geom_point() +
  labs(title = "Percentage Time Spent Foraging vs. gtime", x = "gtime", y = "Percentage Time Foraging")

dpt_pertask <- ggplot(data, aes(x = gtime, y = pertask_foraging)) +
  geom_point() +
  labs(title = "Percentage Task Choce Foraging vs. gtime", x = "gtime", y = "Percentage Time Foraging")


ggsave("OutputImages/deadInd_perTimeForaging.png", dpt_pertime, width = 8, height = 6)
ggsave("OutputImages/deadInd_perTaskForaging.png", dpt_pertask, width = 8, height = 6)
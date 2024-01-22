library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)


directory <- getwd()
files <- list.files(directory, pattern = "_evolution.csv", full.names = TRUE)

# Loop through each file
for (file in files) {
  # Extract simulation ID from the filename
  simulationID <- as.numeric(str_extract(file, "\\d+"))
  data <- read.csv(file)
  cat("Analysing evolution for simulation ID:", simulationID, "\n")
}

# Create output folder if not exists
dir.create("OutputImages", showWarnings = FALSE)

# Plot dispersal_mean vs gtime with standard deviation
p3 <- ggplot(data, aes(x = gtime, y = dispersal_avg, color = "Dispersal Mean")) +
  geom_line() +
  geom_ribbon(aes(ymin = dispersal_avg - dispersal_std, ymax = dispersal_avg + dispersal_std), alpha = 0.3, fill = "green", color = NA) +
  labs(title = "Dispersal Average vs. gtime ", x = "gtime", y = "Dispersal Mean") +
  theme_minimal() +
  theme(legend.position="top") +
  scale_color_manual(values = "green") +
  guides(color = guide_legend(title = NULL))

# Plot gtime vs events
p4 <- ggplot(data, aes(x = event, y = gtime)) +
  geom_line() +
  labs(title = "gtime vs. Events", x = "Events", y = "gtime") +
  theme_minimal()

# Plot total females and total males vs gtime
p5 <- ggplot(data, aes(x = gtime)) +
  geom_line(aes(y = num_female, color = "Total Females"), linetype = "solid") +
  geom_line(aes(y = num_male, color = "Total Males"), linetype = "dashed") +
  labs(title = "Total Females and Males vs. gtime", x = "gtime", y = "Count") +
  theme_minimal() +
  theme(legend.position="top") +
  scale_color_manual(values = c("Total Females" = "blue", "Total Males" = "red")) +
  guides(color = guide_legend(title = NULL, override.aes = list(linetype = c("solid", "dashed"))))

# Plot fem_avg, femLarv_avg, and malLarv_avg with standard deviations vs gtime
p6 <- ggplot(data, aes(x = gtime)) +
  geom_line(aes(y = fem_avg, color = "Fem Avg"), linetype = "solid") +
  geom_line(aes(y = femLarv_avg, color = "FemLarv Avg"), linetype = "solid") +
  geom_line(aes(y = malLarv_avg, color = "MalLarv Avg"), linetype = "solid") +
  labs(title = "Avg and NestWise Fem Avg, FemLarv Avg, and MalLarv Avg vs. gtime", x = "gtime", y = "Average") +
  theme_minimal() +
  theme(legend.position="top") +
  scale_color_manual(values = c("Fem Avg" = "blue", "FemLarv Avg" = "red", "MalLarv Avg" = "green")) +
  guides(color = guide_legend(title = NULL))

# Save individual plots as images in the output folder
ggsave("OutputImages/evolution_choiceIntercept.png", p1, width = 8, height = 6)
ggsave("OutputImages/evolution_choiceSlope.png", p2, width = 8, height = 6)
ggsave("OutputImages/evolution_dispersal.png", p3, width = 8, height = 6)
ggsave("OutputImages/evolution_GtimeVsEvents.png", p4, width = 8, height = 6)
ggsave("OutputImages/evolution_TotalAdults.png", p5, width = 8, height = 6)
ggsave("OutputImages/evolution_NestWisePopulation.png", p6, width = 8, height = 6)
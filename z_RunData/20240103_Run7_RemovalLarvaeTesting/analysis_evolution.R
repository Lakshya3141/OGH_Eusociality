library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)


data <- read.csv("OutputCSV/evolution.csv")

# Create output folder if not exists
dir.create("OutputImages", showWarnings = FALSE)

# Plot choice_int_avg vs gtime with standard deviation
p1 <- ggplot(data, aes(x = gtime, y = choice_int_avg, color = "Choice Intercept")) +
  geom_line() +
  geom_ribbon(aes(ymin = choice_int_avg - choice_int_std, ymax = choice_int_avg + choice_int_std), alpha = 0.3, fill = "blue", color = NA) +  # Remove boundary color
  labs(title = "Choice Intercept Average vs. gtime ", x = "gtime", y = "Choice Intercept Average") +
  theme_minimal() +
  theme(legend.position="top") +
  scale_color_manual(values = "blue") +
  guides(color = guide_legend(title = NULL))


# Plot choice_slope_avg vs gtime with standard deviation
p2 <- ggplot(data, aes(x = gtime, y = choice_slope_avg, color = "Choice Slope")) +
  geom_line() +
  geom_ribbon(aes(ymin = choice_slope_avg - choice_slope_std, ymax = choice_slope_avg + choice_slope_std), alpha = 0.3, fill = "red", color = NA) +
  labs(title = "Choice Slope Average vs. gtime ", x = "gtime", y = "Choice Slope Average") +
  theme_minimal() +
  theme(legend.position="top") +
  scale_color_manual(values = "red") +
  guides(color = guide_legend(title = NULL))

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
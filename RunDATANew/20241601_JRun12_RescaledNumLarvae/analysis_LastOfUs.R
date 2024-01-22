library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)
# # Read the config.ini file
# config_data <- config::get(file = "config.ini")
# 
# # Define the variables you want to extract
# variables_to_extract <- c("dBroodingTime", "dForagingTime")
# 
# # Create a list to store the extracted values
# extracted_values <- list()
# 
# # Extract values for each variable
# for (variable in variables_to_extract) {
#   if (variable %in% names(config_data$params)) {
#     value <- config_data$params[[variable]]
#     extracted_values[[variable]] <- as.numeric(value)
#   } else {
#     warning(paste("Variable", variable, "not found in config.ini"))
#   }
# }


num_nests_lou <- 25
# enter number of nests

# Read the higher mutation v2 CSV file
directory <- getwd()
files <- list.files(directory, pattern = "_LastOfUs.csv", full.names = TRUE)

# Loop through each file
for (file in files) {
  # Extract simulation ID from the filename
  simulationID <- as.numeric(str_extract(file, "\\d+"))
  data_last <- read.csv(file)
  cat("Analysing evolution for simulation ID:", simulationID, "\n")
}

data_last$log_disp <- 1/(1+exp(data_last$dispersal))

# Subset dead individuals (t_death != -1)
dead_individuals <- data_last[data_last$t_death != -1, ]

dummy <- data_last[data_last$t_death == -1, ]
# Subset alive individuals (keep the row with the largest gtime)
alive_individuals <- dummy[!duplicated(dummy$ind_id, fromLast = TRUE), ]
alive_individuals <- alive_individuals[!(alive_individuals$ind_id %in% unique(dead_individuals$ind_id)), ]
# Combine the subsets if needed
combined_data <- rbind(dead_individuals, alive_individuals)

# Calculate lifespans
dead_individuals$lifespan <- dead_individuals$gtime   - dead_individuals$t_birth
combined_data$lifespan <- combined_data$gtime   - combined_data$t_birth

if (sum(alive_individuals$t_birth == 0) > 0) {
  print(paste0(sum(alive_individuals$t_birth == 0)," Individuals from start survived"))
} else {
  print("NO Individuals from the start survived")
}

#### P: Lifespan histogram total dead population ####
# Create a histogram using ggplot2
histogram <- ggplot(dead_individuals, aes(x = lifespan)) +
  geom_histogram(binwidth = 5, fill = "lightblue", color = "black") +
  labs(title = "Histogram of Lifespans (dead individuals)", x = "Lifespan") +
  theme_minimal() +
  theme(legend.position = "none") +
  geom_vline(xintercept = mean(dead_individuals$lifespan, na.rm = TRUE), col = "red", lwd = 2) +
  annotate("text", x = mean(dead_individuals$lifespan, na.rm = TRUE), y = 10,
           label = paste("Average Lifespan:", round(mean(dead_individuals$lifespan, na.rm = TRUE), 2)), col = "red")

# Save the plot
ggsave("OutputImages/LastOfUs_HistDeadLifespan.png", plot = histogram)

#### P: Lifespan histogram total ALL population ####
# Create a histogram using ggplot2
histogram <- ggplot(combined_data, aes(x = lifespan)) +
  geom_histogram(binwidth = 5, fill = "lightblue", color = "black") +
  labs(title = "Histogram of All Lifespans", x = "Lifespan") +
  theme_minimal() +
  theme(legend.position = "none") +
  geom_vline(xintercept = mean(combined_data$lifespan, na.rm = TRUE), col = "red", lwd = 2) +
  annotate("text", x = mean(combined_data$lifespan, na.rm = TRUE), y = 10,
           label = paste("Average Lifespan:", round(mean(combined_data$lifespan, na.rm = TRUE), 2)), col = "red")

# Save the plot
ggsave("OutputImages/LastOfUs_HistAllLifespan.png", plot = histogram)


# Add a line at the average lifespan
avg_lifespan <- mean(dead_individuals$lifespan, na.rm = TRUE)

# Print average lifespan to the console
cat("Average Lifespan:", round(avg_lifespan, 2), "\n")

# Calculate and print average time per generation
total_gtime <- max(data_last$gtime)
generations_passed <- total_gtime / mean(dead_individuals$lifespan)
cat(round(generations_passed, 2), " generations done in ", total_gtime, "gtime \n")


#### Reaction norm graphs ####
alpha <- length(unique(combined_data$ind_id))
# alpha <- 100 # In case of just 100 individuals

# take 200 individuals sampled or however many
if (alpha > 200) {
  alpha <- 200
}

# seed value to be checked and rechanged to simulation ID
set.seed(123)
subset_alpha_data <- combined_data[sample(nrow(combined_data), alpha), ]

# Initialize an empty plot
p <- ggplot() +
  labs(title = paste0("Choice Reaction Norms for ",alpha," individuals in LastOfUs"), x = "Number of Larva", y = "Choice Reaction Norm") +
  theme_minimal() +
  theme(legend.position = "none") +
  ylim(0, 1.1)
# max_larva decides range of graph in output
max_larva = 10
x = seq(0, max_larva, length.out = 100)
# Loop to add logistic curves to the plot
for (i in seq(1:alpha)) {
  curve <- data.frame(x = x, 
                      y = 1 / (1 + exp(subset_alpha_data$choice_int[i] + subset_alpha_data$choice_slope[i] * x)))
  p <- p + geom_line(data = curve, aes(x = x, y = y, group = factor(i)))
}

# Save the plot
ggsave("OutputImages/LastOfUs_ReactionNorms.png", p, width = 8, height = 6)

# Plot a histogram of dispersal values for all individuals
pdisp <- ggplot(combined_data, aes(x = log_disp)) +
  geom_histogram(fill = "lightblue", color = "black") +
  labs(title = "Histogram of Dispersal Values for All LastOfUs", x = "Dispersal", y = "Count") +
  theme_minimal()

# Save the plot
ggsave("OutputImages/LastOfUs_HistAllDispersal.png", pdisp, width = 8, height = 6)

# Find the total number of unique nest IDs
unique_nest_ids <- unique(data_last$nest_id)

# take 25 individuals sampled or however many
sampled_nest_ids <- sample(unique_nest_ids, 25)

# Create a color palette with 25 distinct colors
color_palette <- scales::hue_pal()(25)

# Initialize an empty plot for p7
p7 <- ggplot() +
  labs(title = "Nestwise Adult Female Growth of 25 Sampled Nests", x = "gtime", y = "Number of Adult Females") +
  theme_minimal() +
  theme(legend.position = "none")

# Loop through the sampled nest IDs
for (i in seq_along(sampled_nest_ids)) {
  nest_id <- sampled_nest_ids[i]
  color <- color_palette[i]
  
  # Subset data for the current nest ID
  subset_nest_data <- data_last[data_last$nest_id == nest_id, ]
  
  # Plot num_adults vs gtime for the current nest with a unique color
  p7 <- p7 + geom_line(data = subset_nest_data, aes(x = gtime, y = num_adults, group = factor(nest_id), color = factor(nest_id)), linetype = "solid")
}

# Save the plot
ggsave("OutputImages/LastOfUs_AdultNestGrowth.png", p7, width = 8, height = 6)

#### NestWise Graphs ####

# Randomly select 25 unique nest IDs from dead data, or however many available
if (length(unique(dead_individuals$nest_id)) < 25) {
  num_sampled <- length(unique(dead_individuals$nest_id))
} else {
  num_sampled <- 25
}

random_nest_ids <- sample(unique(dead_individuals$nest_id), num_sampled)

# Initialize a list to store plots
plot_list_pertime <- list()
plot_list_perchoice <- list()
plot_list_lifespan <- list()

# Loop through the randomly selected nest IDs
for (nest_id in sort(random_nest_ids)) {
  # Subset data for the current nest ID
  subset_nest_data <- combined_data[combined_data$nest_id == nest_id, ]
  # time to be replaced
  subset_nest_data$tottime <- subset_nest_data$num_foraging*3.0 + (subset_nest_data$num_choices - subset_nest_data$num_foraging)*4.0
  subset_nest_data$pertimeforaging = subset_nest_data$num_foraging*3.0 / subset_nest_data$tottime * 100
  subset_nest_data$perforaging = subset_nest_data$num_foraging / subset_nest_data$num_choices * 100
  # Create histogram for total time spent foraging
  p8 <- ggplot(subset_nest_data, aes(x = pertimeforaging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Percentage Time Foraging", y = "Proportion") +
    theme_minimal()+
    coord_cartesian(xlim = c(0, 100))
  
  # Save the plot to the list
  plot_list_pertime[[paste("Nest", nest_id, sep = "_")]] <- p8
  
  # Create histogram for task foraging percentage
  p9 <- ggplot(subset_nest_data, aes(x = perforaging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Percentage Choice Foraging", y = "Proportions") +
    theme_minimal()+
    coord_cartesian(xlim = c(0, 100))
  
  # Save the plot to the list
  plot_list_perchoice[[paste("Nest", nest_id, sep = "_")]] <- p9
  
  # Create histogram for task foraging percentage
  p10 <- ggplot(subset_nest_data, aes(x = lifespan)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Lifespan", y = "Proportions") +
    theme_minimal()+
    coord_cartesian(xlim = c(0, 100))
  
  # Save the plot to the list
  plot_list_lifespan[[paste("Nest", nest_id, sep = "_")]] <- p10
}


# Save the combined plot
ggsave(paste("OutputImages/LastOfUs_NWHistTimeForaging.png", sep = ""), wrap_plots(plot_list_pertime), width = 12, height = 8)
ggsave(paste("OutputImages/LastOfUs_NWHistTaskForaging.png", sep = ""), wrap_plots(plot_list_perchoice), width = 12, height = 8)
ggsave(paste("OutputImages/LastOfUs_NWHistLifespan.png", sep = ""), wrap_plots(plot_list_lifespan), width = 12, height = 8)

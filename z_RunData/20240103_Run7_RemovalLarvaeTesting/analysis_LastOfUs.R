library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)

num_nests_lou <- 25
# enter number of nests

# Read the higher mutation v2 CSV file
data_last <- read.csv("OutputCSV/LastOfUs.csv")

# Subset data for individuals with t_death != -1
subset_data <- data_last[data_last$t_death != -1, ]

# Calculate lifespan
subset_data$lifespan <- subset_data$t_death - subset_data$t_birth

# Create a histogram using ggplot2
histogram <- ggplot(subset_data, aes(x = lifespan)) +
  geom_histogram(binwidth = 5, fill = "lightblue", color = "black") +
  labs(title = "Histogram of Lifespans", x = "Lifespan") +
  theme_minimal() +
  theme(legend.position = "none") +
  geom_vline(xintercept = mean(subset_data$lifespan, na.rm = TRUE), col = "red", lwd = 2) +
  annotate("text", x = mean(subset_data$lifespan, na.rm = TRUE), y = 10,
           label = paste("Average Lifespan:", round(mean(subset_data$lifespan, na.rm = TRUE), 2)), col = "red")

# Save the plot
ggsave("OutputImages/LastOfUs_HistLifespan.png", plot = histogram)

# Add a line at the average lifespan
avg_lifespan <- mean(subset_data$lifespan, na.rm = TRUE)

# Print average lifespan to the console
cat("Average Lifespan:", round(avg_lifespan, 2), "\n")

# Calculate and print average time per generation
total_gtime <- max(data_last$gtime)
generations_passed <- total_gtime / mean(subset_data$lifespan)
cat(round(generations_passed, 2), " generations done in ", total_gtime, "gtime \n")

# Find the total number of unique starting individuals
unique_starting_individuals <- data_last[1:1000, ] %>%
  distinct(ind_id) %>%
  arrange(desc(ind_id)) %>%
  slice(which.max(ind_id < 1000))

alpha <- unique_starting_individuals$ind_id + 1
# alpha <- 100 # In case of just 100 individuals

# Take the first alpha rows of output_LastOfUs.csv
subset_alpha_data <- data_last[1:alpha, ]

# Confirm that these have unique ind_ids
stopifnot(length(unique(subset_alpha_data$ind_id)) == nrow(subset_alpha_data))

# Initialize an empty plot
p <- ggplot() +
  labs(title = "Choice Reaction Norms for Sampled LastOfUs", x = "Number of Larva", y = "Choice Reaction Norm") +
  theme_minimal() +
  theme(legend.position = "none") +
  ylim(0, 1.1)
max_larva = 5
x = seq(0, max_larva, length.out = 100)
# Loop to add logistic curves to the plot
for (i in seq(1:alpha)) {
  curve <- data.frame(x = x, 
                      y = 1 / (1 + exp(subset_alpha_data$choice_int[i] + subset_alpha_data$choice_slope[i] * x)))
  p <- p + geom_line(data = curve, aes(x = x, y = y, group = factor(i)))
}

# Save the plot
ggsave("OutputImages/LastOfUs_ReactionNorms.png", p, width = 8, height = 6)

# Plot a histogram of dispersal values for alpha individuals
pdisp <- ggplot(subset_alpha_data, aes(x = dispersal)) +
  geom_histogram(binwidth = 0.1, fill = "lightblue", color = "black") +
  labs(title = "Histogram of Dispersal Values for Sampled LastOfUs", x = "Dispersal", y = "Count") +
  theme_minimal()

# Save the plot
ggsave("OutputImages/LastOfUs_HistDispersal.png", pdisp, width = 8, height = 6)

# Find the total number of unique nest IDs
unique_nest_ids <- unique(data_last$nest_id)

# Randomly sample 100 nest IDs or 20 depending on simulation
# sampled_nest_ids <- sample(unique_nest_ids, 100)
sampled_nest_ids <- unique_nest_ids

# Initialize an empty plot for p7
p7 <- ggplot() +
  labs(title = "Nestwise Adult Female Growth of Sampled Nests", x = "gtime", y = "Number of Adult Females") +
  theme_minimal() +
  theme(legend.position = "none")

# Loop through the sampled nest IDs
for (nest_id in sampled_nest_ids) {
  # Subset data for the current nest ID
  subset_nest_data <- data_last[data_last$nest_id == nest_id, ]
  
  # Plot num_adults vs gtime for the current nest
  p7 <- p7 + geom_line(data = subset_nest_data, aes(x = gtime, y = num_adults, group = factor(nest_id)))
}

# Save the plot
ggsave("OutputImages/LastOfUs_AdultNestwiseGrowth.png", p7, width = 8, height = 6)

##### LoU: Specialisation graphs #####
# Set seed for reproducibility
set.seed(123)

# Randomly select 4 unique nest IDs
# or all 25 for now
random_nest_ids <- unique(data_last$nest_id)

# Initialize a list to store plots
plot_list_pertime <- list()
plot_list_perchoice <- list()

# Loop through the randomly selected nest IDs
for (nest_id in random_nest_ids) {
  # Subset data for the current nest ID
  subset_nest_data <- data_last[data_last$nest_id == nest_id, ]
  
  # Calculate total number of task switches and their percentage for each individual with more than 10 tasks
  unique_individuals <- unique(subset_nest_data$ind_id)
  
  # Initialize data frames to store results
  task_choice_data <- data.frame(ind_id = numeric(),
                                 total_task_choices = numeric(),
                                 pertime_foraging = numeric(),
                                 perchoice_foraging = numeric())
  
  # Loop through unique individual IDs
  for (ind_id in unique_individuals) {
    # Subset data for the current individual ID
    subset_ind_data <- subset_nest_data[subset_nest_data$ind_id == ind_id, ]
    
    # Check if the individual has more than 10 tasks
    if (nrow(subset_ind_data) > 10) {
      # Calculate total number of task switches
      total_task_choices <- nrow(subset_ind_data)
      
      # Calculate total time spent foraging
      pertime_foraging <- sum(subset_ind_data$current_foraging * 3.0) / sum(subset_ind_data$current_foraging * 3.0 + (1 - subset_ind_data$current_foraging) * 4.0)
      
      # Calculate task foraging percentage
      perchoice_foraging <- sum(subset_ind_data$current_foraging) / nrow(subset_ind_data)
      
      # Append results to the data frame
      task_choice_data <- rbind(task_choice_data,
                                data.frame(ind_id = ind_id,
                                           total_task_choices = total_task_choices,
                                           pertime_foraging = pertime_foraging,
                                           perchoice_foraging = perchoice_foraging))
    }
  }
  
  # Create histogram for total time spent foraging
  p8 <- ggplot(task_choice_data, aes(x = pertime_foraging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Total Time Foraging", y = "Count") +
    theme_minimal()
  
  # Save the plot to the list
  plot_list_pertime[[paste("Nest", nest_id, sep = "_")]] <- p8
  
  # Create histogram for task foraging percentage
  p9 <- ggplot(task_choice_data, aes(x = perchoice_foraging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Task Foraging Percentage", y = "Count") +
    theme_minimal()
  
  # Save the plot to the list
  plot_list_perchoice[[paste("Nest", nest_id, sep = "_")]] <- p9
}


# Save the combined plot
ggsave(paste("OutputImages/LastOfUs_HistTimeForaging.png", sep = ""), wrap_plots(plot_list_pertime), width = 12, height = 8)
ggsave(paste("OutputImages/LastOfUs_HistTaskForaging.png", sep = ""), wrap_plots(plot_list_perchoice), width = 12, height = 8)

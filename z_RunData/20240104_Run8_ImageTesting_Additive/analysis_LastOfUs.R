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
unique_starting_individuals <- data_last %>%
  distinct(ind_id) %>%
  arrange(desc(ind_id)) %>%
  slice(which.max(ind_id < 1000))

alpha <- length(unique(subset_data$ind_id))
# alpha <- 100 # In case of just 100 individuals

# take 200 individuals sampled or however many
if (0.1*alpha > 200) {
  alpha <- 200
} else {
  alpha <- 0.1*alpha
}

# seed value to be checked and rechanged to simulation ID
set.seed(123)
subset_alpha_data <- subset_data[sample(nrow(subset_data), alpha), ]

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

# Plot a histogram of dispersal values for alpha individuals
pdisp <- ggplot(subset_alpha_data, aes(x = dispersal)) +
  geom_histogram(fill = "lightblue", color = "black") +
  labs(title = "Histogram of Dispersal Values for Sampled LastOfUs", x = "Dispersal", y = "Count") +
  theme_minimal()

# Save the plot
ggsave("OutputImages/LastOfUs_HistDispersal.png", pdisp, width = 8, height = 6)

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
ggsave("OutputImages/LastOfUs_AdultNestwiseGrowth.png", p7, width = 8, height = 6)

##### LoU: Specialisation graphs #####

# Randomly select 25 unique nest IDs from dead data, or however many available
if (length(unique(subset_data$nest_id)) < 25) {
  num_sampled <- length(unique(subset_data$nest_id))
} else {
  num_sampled <- 25
}

random_nest_ids <- sample(unique(subset_data$nest_id), num_sampled)

# Initialize a list to store plots
plot_list_pertime <- list()
plot_list_perchoice <- list()

# Loop through the randomly selected nest IDs
for (nest_id in sort(random_nest_ids)) {
  # Subset data for the current nest ID
  subset_nest_data <- subset_data[subset_data$nest_id == nest_id, ]
  # time to be replaced
  subset_nest_data$tottime <- subset_nest_data$num_foraging*3.0 + (subset_nest_data$num_choices - subset_nest_data$num_foraging)*4.0
  subset_nest_data$pertimeforaging = subset_nest_data$num_foraging*3.0 / subset_nest_data$tottime * 100
  subset_nest_data$perforaging = subset_nest_data$num_foraging / subset_nest_data$num_choices * 100
  # Create histogram for total time spent foraging
  p8 <- ggplot(subset_nest_data, aes(x = pertimeforaging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Percentage Time Foraging", y = "Proportion") +
    theme_minimal()
  
  # Save the plot to the list
  plot_list_pertime[[paste("Nest", nest_id, sep = "_")]] <- p8
  
  # Create histogram for task foraging percentage
  p9 <- ggplot(subset_nest_data, aes(x = perforaging)) +
    geom_histogram(fill = "lightblue", color = "black") +
    labs(title = paste("Nest ID:", nest_id), x = "Percentage Choice Foraging", y = "Proportions") +
    theme_minimal()
  
  # Save the plot to the list
  plot_list_perchoice[[paste("Nest", nest_id, sep = "_")]] <- p9
}


# Save the combined plot
ggsave(paste("OutputImages/LastOfUs_HistTimeForaging.png", sep = ""), wrap_plots(plot_list_pertime), width = 12, height = 8)
ggsave(paste("OutputImages/LastOfUs_HistTaskForaging.png", sep = ""), wrap_plots(plot_list_perchoice), width = 12, height = 8)

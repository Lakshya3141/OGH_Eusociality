library(ggplot2)
library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)

# Step 1: Classifying data
mapping <- read.csv("mapping_condition.csv")
conditions_to_compare <- c("lB_lF", "mB_lF", "hB_lF")  # Add the conditions you want to compare

#### Evolution data graphs ####
combined_data <- data.frame()
for (dum in seq(1:dim(mapping)[1])) {
  folder <- mapping$X[dum]
  condition <- mapping$condition[mapping$X == folder]
  
  # Check if condition is not NA
  if (mapping$evolution[dum]) {
    # Generate file name pattern
    file_pattern <- "_evolution.csv"
    
    # List files in the folder
    files <- list.files(path = paste0("./", folder), pattern = file_pattern, full.names = TRUE)
    
    # Check if files are found
    if (length(files) > 0) {
      # Read the first file (assuming only one file matches the pattern)
      data <- read.csv(files[1])
      
      data$rep <- mapping$rp[mapping$X == folder]
      data$cond <- condition
      
      # Combine with previous data or create a new dataset
      if (exists("combined_data")) {
        combined_data <- rbind(combined_data, data)
      } else {
        combined_data <- data
      }
    }
  }
}
combined_data$prob_avg <- 1/(1+exp(combined_data$dispersal_avg))
combined_data$prob_ul <- 1/(1+exp(combined_data$dispersal_avg + combined_data$dispersal_std))
combined_data$prob_ll <- 1/(1+exp(combined_data$dispersal_avg - combined_data$dispersal_std))

valid_conditions <- c()
# Create a list to store plots for each condition
plots_dispersal <- list()
plots_choiceInt <- list()
plots_choiceSlope <- list()

int_ul = max(combined_data$choice_int_avg + combined_data$choice_int_std)*1.1
int_ll = min(combined_data$choice_int_avg - combined_data$choice_int_std)*1.2
slope_ul = max(combined_data$choice_slope_avg + combined_data$choice_slope_std)*1.01
slope_ll = min(combined_data$choice_slope_avg - combined_data$choice_slope_std)*1.01


# Loop through each condition
for (condition in conditions_to_compare) {
  # Subset the combined_data for the specific condition
  if (condition %in% unique(mapping$condition)) {
    append(valid_conditions, condition)
    subset_data <- combined_data[combined_data$cond == condition, ]
    
    ##### Dispersal plot #####
    # Create a plot for the current condition
    plot <- ggplot(subset_data, aes(x = gtime, y = prob_avg, color = factor(rep))) +
      geom_line(linewidth = 1.0) +
      geom_ribbon(aes(ymin = prob_ll, ymax = prob_ul, fill = factor(rep)), 
                  alpha = 0.3, color = NA, show.legend = FALSE) +
      labs(title = condition, x = "Time", y = "Dispersal probability") +
      theme_minimal() +
      theme(legend.position = "top", plot.title = element_text(size = 15, hjust = 0.5)) +
      ylim(0, 1) +
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    # Add the plot to the list
    plots_dispersal[[condition]] <- plot
    
    #### ChoiceInt plot ####
    
    
    plot <- ggplot(subset_data, aes(x = gtime, y = choice_int_avg, color = factor(rep))) +
      geom_line(linewidth = 1.0) +
      geom_ribbon(aes(ymin = choice_int_avg - choice_int_std, ymax = choice_int_avg + choice_int_std, fill = factor(rep)), 
                  alpha = 0.3, color = NA, show.legend = FALSE) +
      labs(x = "time", y = "Reaction norm intercept") +
      theme_minimal() +
      theme(legend.position = "top") +
      # scale_color_viridis_d() +
      # scale_fill_viridis_d() +
      ylim(int_ll, int_ul) +
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    # Add the plot to the list
    plots_choiceInt[[condition]] <- plot
    
    #### Choice Slope plot ####
    plot <- ggplot(subset_data, aes(x = gtime, y = choice_slope_avg, color = factor(rep))) +
      geom_line(linewidth = 1.0) +
      geom_ribbon(aes(ymin = choice_slope_avg - choice_slope_std, ymax = choice_slope_avg + choice_slope_std, fill = factor(rep)), 
                  alpha = 0.3, color = NA, show.legend = FALSE) +
      labs(x = "time", y = "Reaction norm slope") +
      theme_minimal() +
      theme(legend.position = "top") +
      # scale_color_viridis_d() +
      # scale_fill_viridis_d() +
      ylim(slope_ll, slope_ul) +
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    # Add the plot to the list
    plots_choiceSlope[[condition]] <- plot
  }
}


#### dead Individual data graphs ####
combined_data <- data.frame()
for (dum in seq(1:dim(mapping)[1])) {
  folder <- mapping$X[dum]
  condition <- mapping$condition[mapping$X == folder]
  
  # Check if condition is not NA
  if (mapping$deadInd[dum]) {
    # Generate file name pattern
    file_pattern <- "_deadIndividuals.csv"
    
    # List files in the folder
    files <- list.files(path = paste0("./", folder), pattern = file_pattern, full.names = TRUE)
    
    # Check if files are found
    if (length(files) > 0) {
      # Read the first file (assuming only one file matches the pattern)
      data <- read.csv(files[1])
      data <- subset(data, num_choices > 40)
      data$rep <- mapping$rp[mapping$X == folder]
      data$cond <- condition
      
      # Combine with previous data or create a new dataset
      if (exists("combined_data")) {
        combined_data <- rbind(combined_data, data)
      } else {
        combined_data <- data
      }
    }
  }
}

combined_data$pertask_foraging = combined_data$num_foraging/combined_data$num_choices * 100
combined_data$pertime_foraging = combined_data$pertime_foraging*100
valid_conditions <- c()
# Create a list to store plots for each condition
plots_taskChoice <- list()
plots_perForaging <- list()

# Loop through each condition
for (condition in conditions_to_compare) {
  # Subset the combined_data for the specific condition
  if (condition %in% unique(mapping$condition)) {
    append(valid_conditions, condition)
    subset_data <- combined_data[combined_data$cond == condition, ]
    
    ##### Dot plot #####
    # Create a plot for the current condition
    dpt_pertime <- ggplot(subset_data, aes(x = gtime, y = pertask_foraging, color = factor(rep))) +
      geom_point(size = 0.7) +
      labs(x = "time", y = "Percentage Time Foraging") +
      theme_minimal() +
      theme(legend.position = "top") +
      ylim(0,100) +
      # scale_color_viridis_d() +
      # scale_fill_viridis_d() +
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    dpt_pertask <- ggplot(subset_data, aes(x = gtime, y = pertask_foraging, color = factor(rep))) +
      geom_point(size = 0.7) +
      labs(x = "time", y = "Percentage Foraging") +
      theme_minimal() +
      theme(legend.position = "top") +
      ylim(0,100) +
      # scale_color_viridis_d() +
      # scale_fill_viridis_d() +
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    
    # Add the plot to the list
    plots_taskChoice[[condition]] <- dpt_pertask
    plots_perForaging[[condition]] <- dpt_pertime
  }
}



#### Last of US data graphs ####
combined_data <- data.frame()
for (dum in seq(1:dim(mapping)[1])) {
  folder <- mapping$X[dum]
  condition <- mapping$condition[mapping$X == folder]
  
  # Check if condition is not NA
  if (mapping$lou[dum]) {
    # Generate file name pattern
    file_pattern <- "_LastOfUs.csv"
    
    # List files in the folder
    files <- list.files(path = paste0("./", folder), pattern = file_pattern, full.names = TRUE)
    
    # Check if files are found
    if (length(files) > 0) {
      # Read the first file (assuming only one file matches the pattern)
      data <- read.csv(files[1])
      data$rep <- mapping$rp[mapping$X == folder]
      data$cond <- condition
      
      # Combine with previous data or create a new dataset
      if (exists("combined_data")) {
        combined_data <- rbind(combined_data, data)
      } else {
        combined_data <- data
      }
    }
  }
}


# Subset dead individuals (t_death != -1)
dead_individuals <- combined_data[combined_data$t_death != -1, ]

alive_individuals <- combined_data[combined_data$t_death == -1, ]
# Subset alive individuals (keep the row with the largest gtime)
alive_individuals <- alive_individuals[!duplicated(alive_individuals$ind_id, fromLast = TRUE), ]
alive_individuals <- alive_individuals[!(alive_individuals$ind_id %in% unique(dead_individuals$ind_id)), ]
# Combine the subsets if needed
all_individuals <- rbind(dead_individuals, alive_individuals)
valid_conditions <- c()

num_nests_lou <- 25
# Create a list to store plots for each condition
plots_reactionNorms <- list()
max_larva = 200
x = seq(0, max_larva, length.out = 100)
# Loop through each condition
# Loop through each condition
valid_conditions <- c()
dum_color <- c("blue", "red", "green")
colors_vector <- c("#FF0000", "#0000FF", "#00FF00", "#000000", "#FFC0CB")
# Create a vector of unique colors for replicates
replicate_colors <- viridis::viridis_pal()(length(unique(subset_data$rep)))

for (condition in conditions_to_compare) {
  # Subset the combined_data for the specific condition
  if (condition %in% unique(mapping$condition)) {
    valid_conditions <- c(valid_conditions, condition)
    subset_data <- combined_data[combined_data$cond == condition, ]
    
    ##### Reaction norms #####
    
    p <- ggplot() +
      labs(title = paste0("Evolved reaction norm"), x = "Number of Larvae", y = "Prob foraging choice") +
      theme_minimal() +
      theme(legend.position = "none") +
      ylim(0, 1.1) +
      theme_minimal() +
      theme(legend.position = "top") +
      # scale_color_manual(values = replicate_colors) +  # Set custom colors for replicates
      guides(color = guide_legend(title = "Rep"),
             fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
    
    for (replic in unique(subset_data$rep)) {
      rep_subset <- subset_data[subset_data$rep == replic, ]
      alpha <- length(unique(rep_subset$ind_id))
      if (alpha > 200) {
        alpha <- 200
      }
      set.seed(123)
      subset_alpha_data <- rep_subset[sample(nrow(rep_subset), alpha), ]
      color_value <- colors_vector[replic]
      for (i in seq(1:alpha)) {
        curve <- data.frame(x = x, 
                            y = 1 / (1 + exp(subset_alpha_data$choice_int[i] + subset_alpha_data$choice_slope[i] * x)))
        p <- p + geom_line(data = curve, aes(x = x, y = y) , color = factor(replic), linewidth = 0.8)
      }
    }
    
    # Add the plot to the list
    plots_reactionNorms[[condition]] <- p
  }
}

# Combine the plots for each category into a list
all_plots_list <- list(
  plots_dispersal, 
  plots_reactionNorms, 
  plots_perForaging, 
  plots_choiceInt, 
  plots_choiceSlope
)
plot_titles <- c("Dispersal Plot", "Reaction Norms", "% Foraging", "Choice Intercept", "Choice Slope")

# Create a list to store the final plots for each category
final_plots_list <- list()

# Loop through each category and arrange the plots in a grid
for (i in seq_along(all_plots_list)) {
  category_plots <- all_plots_list[[i]]
  
  # Arrange the plots in a grid
  category_grid <- grid.arrange(grobs = category_plots, ncol = length(conditions_to_compare), 
                                top = plot_titles[i])
  
  # Add the arranged grid to the final list
  final_plots_list[[i]] <- category_grid
}

#### Combining plots ####
final_plot_dispersal <- wrap_plots(plots_dispersal, ncol = length(conditions_to_compare),
                                   guides = 'keep') +
  plot_annotation(title = "Dispersal Plots")

final_plot_reactionNorms <- wrap_plots(plots_reactionNorms, ncol = length(conditions_to_compare),
                                       guides = 'keep') +
  plot_annotation(title = "Reaction Norms")

final_plot_perForaging <- wrap_plots(plots_perForaging, ncol = length(conditions_to_compare),
                                    guides = 'keep') +
  plot_annotation(title = "Task Choice Plot")

final_plot_choiceInt <- wrap_plots(plots_choiceInt, ncol = length(conditions_to_compare),
                                   guides = 'keep') +
  plot_annotation(title = "Choice Intercept Plot")

final_plot_choiceSlope <- wrap_plots(plots_choiceSlope, ncol = length(conditions_to_compare),
                                     guides = 'keep') +
  plot_annotation(title = "Choice Slope Plot")

# Combine all final plots
final_combined_plot <- wrap_plots(
  final_plot_dispersal + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_reactionNorms + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_perForaging + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_choiceInt + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_choiceSlope + plot_layout(ncol = length(conditions_to_compare)),
  ncol = 1
)

# Display the final combined plot

ggsave("VaryingThreshold_MidVariance.png", final_combined_plot, height = 15, width = 15, units = "in", dpi = 300)
# ggsave("JaiMataDi.png", final_combined_plot, height = 15, width = 15, units = "in", dpi = 300)


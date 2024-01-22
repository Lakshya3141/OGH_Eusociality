library(ggplot2)
library(tidyverse)
library(ggpubr)
library(gridExtra)
library(patchwork)
library(diptest)
library(MetBrewer)


# Step 1: Classifying data
mapping <- read.csv("mapping_condition.csv")
conditions_to_compare <- c("lb_lf", "mb_lf")  # Add the conditions you want to compare

#### Evolution data graphs ####
combined_data <- data.frame()
for (dum in seq(1:dim(mapping)[1])) {
  folder <- mapping$X[dum]
  condition <- mapping$condition[mapping$X == folder]
  print(condition)
  # Check if condition is not NA
  if (mapping$evolution[dum] && (condition %in% conditions_to_compare)) {
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
  }
}

##### Additional evolution graphs #####

# Additional variables to consider
additional_variables <- c("num_female", "num_male", "fem_avg", "femLarv_avg")

# Corresponding titles for each variable
variable_titles <- c("Number of Females", "Number of Males", "Female Average", "Female Larval Average")

# Create a list to store plots for each additional variable
plots_additional <- list()

# Loop through each additional variable
for (i in seq_along(additional_variables)) {
  variable <- additional_variables[i]
  title <- variable_titles[i]
  
  # Create a list to store plots for each condition
  plots_variable <- list()
  
  # Loop through each condition
  for (condition in conditions_to_compare) {
    # Subset the combined_data for the specific condition
    if (condition %in% unique(mapping$condition)) {
      append(valid_conditions, condition)
      subset_data <- combined_data[combined_data$cond == condition, ]
      
      ## Create plot for the current condition and variable ##
      # Adjust aes() and geom_line() accordingly based on the variable
      plot <- ggplot(subset_data, aes(x = gtime, y = get(variable), color = factor(rep))) +
        geom_line(linewidth = 1.0) +
        labs(title = paste(condition, "-", title), x = "Time", y = title) +
        theme_minimal() +
        theme(legend.position = "top", plot.title = element_text(size = 15, hjust = 0.5)) +
        guides(color = guide_legend(title = "Rep"),
               fill = guide_legend(override.aes = list(color = NULL)))  # Remove fill from the legend
      
      # Add the plot to the list
      plots_variable[[condition]] <- plot
    }
  }
  
  # Add the list of plots for the variable to the overall list
  plots_additional[[variable]] <- plots_variable
}


#### dead Individual data graphs ####
combined_data <- data.frame()
for (dum in seq(1:dim(mapping)[1])) {
  folder <- mapping$X[dum]
  condition <- mapping$condition[mapping$X == folder]
  
  # Check if condition is not NA
  if (mapping$deadInd[dum] && (condition %in% conditions_to_compare)) {
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
  if (mapping$lou[dum] && (condition %in% conditions_to_compare)) {
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

# Loop through each condition
# Loop through each condition
valid_conditions <- c()
dum_color <- c("blue", "red", "green")
colors_vector <- c("#FF0000", "#0000FF", "#00FF00", "#000000", "#FFC0CB")
# Create a vector of unique colors for replicates
# replicate_colors <- viridis::viridis_pal()(length(unique(subset_data$rep)))
subset_data <- data.frame()
plots_react <- list()

##### Reaction norms #####
# Create an empty list to store plots
plots_react <- list()

# Number of replicates
num_replicates <- 3  # Change this number based on your requirement

for (condition in conditions_to_compare) {
  # Subset the combined_data for the specific condition
  if (condition %in% unique(mapping$condition)) {
    valid_conditions <- c(valid_conditions, condition)
    subset_data <- all_individuals[all_individuals$cond == condition, ]
    
    # Create a list to store plots for each replicate
    plots_replicates <- list()
    
    for (replic in unique(subset_data$rep)) {
      print(paste(condition, " : ", replic))
      folder <- mapping$X[mapping$condition == condition & mapping$rp == replic]
      basis_pattern <- "_basis.csv"
      basis_files <- list.files(path = paste0("./", folder), pattern = basis_pattern, full.names = TRUE)
      # Check if files are found
      if (length(basis_files) > 0) {
        # Read the first file (assuming only one file matches the pattern)
        basis <- data.frame(read.csv(basis_files[1], header = FALSE))
        basis <- basis[,-6]
      }
      
      rep_subset <- subset_data[subset_data$rep == replic, ]
      betas <- rep_subset %>% dplyr::select(beta0, beta1, beta2, beta3, beta4) # select gene values 1-5, which are the gene values for the larval reaction norm
      
      time <- Sys.time() # record system time
      set.seed(time) # use time to seed the rng
      
      number_larvae <- length(unique(rep_subset$ind_id))
      if (number_larvae > 100) {
        number_larvae <- 100
      }
      number_larvae <- 25
      subset_betas <- betas[sample(nrow(betas), number_larvae, replace = FALSE), ] # sample random 100 larvae
      mat_results <- matrix(NA, ncol = 2, nrow = nrow(basis) * number_larvae)
      
      # number_larvae <- 10
      # evaluate spline
      upper_loop_limit <- nrow(basis) * number_larvae
      for (i in c(1:upper_loop_limit)) {
        # multiply each of the rows of the basis matrix with the betas (gene values) as a column vector
        # this occurs first by the 100 different nutrition levels that have been precalculated
        # and then by the different larvae
        mat_results[i,1] <- as.matrix(basis[i - ((i - 1) %/% nrow(basis) * nrow(basis)), ]) %*% t(as.matrix(subset_betas[(i - 1 + nrow(basis)) %/% nrow(basis), ]))
        # record an id for the larvae
        mat_results[i,2] <- (i - 1 + nrow(basis)) %/% nrow(basis)
      }
      larval_level <- rep(basis$V2, number_larvae) # predetermined steps of x (nutrition level) to the data frame. this can be read from X0 of the basis matrix
      data_plotting <- cbind(larval_level, mat_results) # combine these nutrition levels with the spline evaluation
      data_plotting <- as.data.frame(data_plotting) # make a data frame
      data_plotting$logistic_vals <- 1/(1+exp(data_plotting$V2))
      color_value <- colors_vector[replic]
      data_plotting$V3 <- as.factor(data_plotting$V3)
      data_plotting$larval_level <- data_plotting$larval_level*500.0
      p <- ggplot(data_plotting, aes(x=larval_level, y=logistic_vals, group=V3)) +
        stat_summary(aes(x=larval_level, y=logistic_vals, group=V3), fun=mean, geom="line", linewidth=1.5, alpha=0.8, col = "grey") +
        scale_y_continuous(name="Foraging probability", limits = c(0.0, 1.0), breaks = seq(0.0, 1.0, 0.2)) +
        scale_x_continuous(name="Larval levels", limits = c(0.0, 500.0), breaks = seq(0.0,500.0,50.0)) +
        theme_minimal() +
        theme(
          axis.text = element_text(size=11,face="plain",color="black"),
          axis.title = element_text(size = 13),
          axis.line = element_line(color="black", linewidth = 0.6),
          panel.border = element_rect(colour = "darkgray", fill=NA, linetype=0.5)
        ) +
        labs(title = paste0(condition, " : ", replic))
      
      # Add the plot to the list for replicates
      plots_replicates[[as.character(replic)]] <- p
    }
    
    # Add the list of plots for replicates to the overall list
    plots_react[[condition]] <- plots_replicates
  }
}

#### Combining plots ####
##### Dispersal + foraging #####
plot_titles <- c("Dispersal Plot", "% Foraging")

final_plot_dispersal <- wrap_plots(plots_dispersal, ncol = length(conditions_to_compare),
                                   guides = 'keep') +
  plot_annotation(title = "Dispersal Plots")

final_plot_perForaging <- wrap_plots(plots_perForaging, ncol = length(conditions_to_compare),
                                     guides = 'keep') +
  plot_annotation(title = "Task Choice Plot")

# Combine all final plots
final_combined_plot <- wrap_plots(
  final_plot_dispersal + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_perForaging + plot_layout(ncol = length(conditions_to_compare)),
  ncol = 1
)

# Display the final combined plot

ggsave("DispersalAndForaging.png", final_combined_plot, height = 15, width = 15, units = "in", dpi = 300)
# ggsave("JaiMataDi.png", final_combined_plot, height = 15, width = 15, units = "in", dpi = 300)


##### Additional graphs #####

plot_titles <- c("Total female population", "Total male population", "Average Nest size", "Average femLarva")

# Create a list to store the final plots for each category

final_plot_1 <- wrap_plots(plots_additional$num_female, ncol = length(conditions_to_compare),
                                   guides = 'keep') +
  plot_annotation(title = plot_titles[1])

final_plot_2 <- wrap_plots(plots_additional$num_male, ncol = length(conditions_to_compare),
                                     guides = 'keep') +
  plot_annotation(title = plot_titles[2])

final_plot_3 <- wrap_plots(plots_additional$fem_avg, ncol = length(conditions_to_compare),
                                   guides = 'keep') +
  plot_annotation(title = plot_titles[3])

final_plot_4 <- wrap_plots(plots_additional$femLarv_avg, ncol = length(conditions_to_compare),
                                     guides = 'keep') +
  plot_annotation(title = plot_titles[4])



# Combine all final plots
final_combined_plot <- wrap_plots(
  final_plot_1 + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_2 + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_3 + plot_layout(ncol = length(conditions_to_compare)),
  final_plot_4 + plot_layout(ncol = length(conditions_to_compare)),
  ncol = 1
)

# Display the final combined plot

ggsave("AdditionalPlots.png", final_combined_plot, height = 15, width = 15, units = "in", dpi = 300)

##### Reaction norms #####
# Number of rows and columns in the grid
num_rows <- num_replicates
num_cols <- length(conditions_to_compare)

# Create an empty list to store plots
plots_list <- vector("list", length = num_rows * num_cols)

# Create a predefined grid
grid <- matrix(plots_list, nrow = num_rows, ncol = num_cols)
rownames(grid) <- seq(1:num_replicates)
colnames(grid) <- conditions_to_compare
mapping$condition[is.na(mapping$condition)] <- "nodata"

# Double for loop to add plots to the grid
for (cond in conditions_to_compare) {
  for (rep in 1:num_replicates) {
    current_plot <- NULL
    if (any(mapping$lou[mapping$rp == rep & mapping$condition == cond] == 1)) {
      # Create your individual plot (replace this with your own plot creation code)
      current_plot <- plots_react[[cond]][[as.character(rep)]]
    } else {
      current_plot <- ggplot() + 
        theme_minimal() +
        theme(axis.title = element_text(size = 13),
          axis.line = element_line(color="black", linewidth = 0.6),
          panel.border = element_rect(colour = "darkgray", fill=NA, linetype=0.5)
        )
      }
    # Add the plot to the grid
    grid[rep, cond] <- list(current_plot)
  }
}

# Arrange the grid using grid.arrange
final_grid_plot <- grid.arrange(grobs = grid, ncol = num_cols)
# Add a title to the final_grid_plot
ggsave("EvolvedReactionNorms.png", final_grid_plot, height = 15, width = 15, units = "in", dpi = 300)

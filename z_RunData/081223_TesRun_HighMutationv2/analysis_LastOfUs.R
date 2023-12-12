# Read the higher mutation v2 CSV file
data <- read.csv("output_LastOfUs.csv")

# Subset data for individuals with t_death != -1
subset_data <- data[data$t_death != -1, ]

# Calculate lifespan
subset_data$lifespan <- subset_data$t_death - subset_data$t_birth

# Plot histogram
hist(subset_data$lifespan, main="Histogram of Lifespans", xlab="Lifespan", col="lightblue", border="black")

# Add a line at the average lifespan
avg_lifespan <- mean(subset_data$lifespan, na.rm = TRUE)
abline(v = avg_lifespan, col="red", lwd=2)

# Print average lifespan on the graph
text(avg_lifespan, 10, labels = paste("Average Lifespan:", round(avg_lifespan, 2)), col="red")

# Print average lifespan to the console
cat("Average Lifespan:", round(avg_lifespan, 2), "\n")

# Calculate and print average time per generation
total_gtime <- max(data$gtime)
generations_passed <- total_gtime / mean(subset_data$lifespan)
cat(round(generations_passed, 2), " generations done in ", total_gtime, "gtime \n")

# So total number of generations in long model are
evolution_numGen = 100000 / avg_lifespan
cat(evolution_numGen ," generations done in long evolutiono of 100000 gtime \n")

# Display the plot
# dev.new()

# Save the plot as an image file if needed
# png("lifespan_histogram.png")
# hist(subset_data$lifespan, main = "Histogram of Lifespans", xlab = "Lifespan", ylab = "Frequency", col = "lightblue", border = "black")
# dev.off()

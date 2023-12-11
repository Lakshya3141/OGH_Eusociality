# Install and load necessary libraries
# install.packages("tidyverse")
# library(tidyverse)

# Read the CSV file
data <- read.csv("output_evolution.csv")

# Plot choice_int_avg vs gtime
ggplot(data, aes(x = gtime, y = choice_int_avg)) +
  geom_line() +
  labs(title = "Choice Intercept Average vs. gtime", x = "gtime", y = "Choice Intercept Average")

# Plot choice_slope_avg vs gtime
ggplot(data, aes(x = gtime, y = choice_slope_avg)) +
  geom_line() +
  labs(title = "Choice Slope Average vs. gtime", x = "gtime", y = "Choice Slope Average")

# Plot dispersal_mean vs gtime
ggplot(data, aes(x = gtime, y = dispersal_mean)) +
  geom_line() +
  labs(title = "Dispersal Mean vs. gtime", x = "gtime", y = "Dispersal Mean")

# Plot gtime vs events
ggplot(data, aes(x = event, y = gtime)) +
  geom_line() +
  labs(title = "gtime vs. Events", x = "Events", y = "gtime")

# Plot choice_int_avg vs events
ggplot(data, aes(x = event, y = choice_int_avg)) +
  geom_line() +
  labs(title = "Choice Intercept Average vs. Events", x = "Events", y = "Choice Intercept Average")

# Plot choice_slope_avg vs events
ggplot(data, aes(x = event, y = choice_slope_avg)) +
  geom_line() +
  labs(title = "Choice Slope Average vs. Events", x = "Events", y = "Choice Slope Average")

# Plot dispersal_mean vs events
ggplot(data, aes(x = event, y = dispersal_mean)) +
  geom_line() +
  labs(title = "Dispersal Mean vs. Events", x = "Events", y = "Dispersal Mean")



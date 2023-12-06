# Load required libraries
library(ggplot2)
library(dplyr)

# Read the CSV file
data <- read.csv("output_million.csv")

# Plot choice_int_avg vs gtime
plot_choice_int <- ggplot(data, aes(x = gtime, y = choice_int_avg)) +
  geom_line() +
  geom_ribbon(aes(ymin = choice_int_avg - choice_int_std, ymax = choice_int_avg + choice_int_std), alpha = 0.2) +
  labs(title = "Choice Intercept vs Time", x = "gtime", y = "Choice Intercept")

# Plot choice_slope_avg vs gtime
plot_choice_slope <- ggplot(data, aes(x = gtime, y = choice_slope_avg)) +
  geom_line() +
  geom_ribbon(aes(ymin = choice_slope_avg - choice_slope_std, ymax = choice_slope_avg + choice_slope_std), alpha = 0.2) +
  labs(title = "Choice Slope vs Time", x = "gtime", y = "Choice Slope")

# Plot dispersal_mean vs gtime
plot_dispersal <- ggplot(data, aes(x = gtime, y = dispersal_mean)) +
  geom_line() +
  geom_ribbon(aes(ymin = dispersal_mean - dispersal_std, ymax = dispersal_mean + dispersal_std), alpha = 0.2) +
  labs(title = "Dispersal Mean vs Time", x = "gtime", y = "Dispersal Mean")

# Plot choice_int_avg, choice_slope_avg, and dispersal_mean vs events
plot_choice_events <- ggplot(data, aes(x = events)) +
  geom_line(aes(y = choice_int_avg), color = "red", linetype = "solid") +
  geom_ribbon(aes(ymin = choice_int_avg - choice_int_std, ymax = choice_int_avg + choice_int_std), fill = "red", alpha = 0.2) +
  geom_line(aes(y = choice_slope_avg), color = "blue", linetype = "solid") +
  geom_ribbon(aes(ymin = choice_slope_avg - choice_slope_std, ymax = choice_slope_avg + choice_slope_std), fill = "blue", alpha = 0.2) +
  geom_line(aes(y = dispersal_mean), color = "green", linetype = "solid") +
  geom_ribbon(aes(ymin = dispersal_mean - dispersal_std, ymax = dispersal_mean + dispersal_std), fill = "green", alpha = 0.2) +
  labs(title = "Genetic Values vs Events", x = "Events", y = "Genetic Values")

# Plot gtime vs events
plot_gtime_events <- ggplot(data, aes(x = events, y = gtime)) +
  geom_line() +
  labs(title = "gtime vs Events", x = "Events", y = "gtime")

# Arrange and print the plots
multiplot <- function(..., plotlist=NULL, file, cols=1, layout=NULL) {
  require(grid)
  
  # Make a list from the ... arguments and plotlist
  plots <- c(list(...), plotlist)
  
  numPlots = length(plots)
  
  # If layout is NULL, then use 'cols' to determine layout
  if (is.null(layout)) {
    # Make the panel
    # ncol: Number of columns of plots
    layout <- matrix(seq(1, cols * ceiling(numPlots/cols)),
                     ncol = cols, nrow = ceiling(numPlots/cols))
  }
  
  if (numPlots==1) {
    print(plots[[1]])
    
  } else {
    # Set up the page
    grid.newpage()
    pushViewport(viewport(layout = grid.layout(nrow(layout), ncol(layout))))
    
    # Make each plot, in the correct location
    for (i in 1:numPlots) {
      # Get the i,j matrix positions of the regions that contain this subplot
      matchidx <- as.data.frame(which(layout == i, arr.ind = TRUE))
      
      print(plots[[i]], vp = viewport(layout.pos.row = matchidx$row,
                                      layout.pos.col = matchidx$col))
    }
  }
}

# Display the plots
multiplot(plot_choice_int, plot_choice_slope, plot_dispersal, plot_choice_events, plot_gtime_events, cols = 2)


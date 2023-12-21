options(scipen = 999)

create_config <- function(config_file_name = "config.ini",
                          dLarvaMatureSize = 1.0,
                          
                          dForagingTime = 3.0,
                          dBroodingTime = 4.0,
                          dDeathTime = 2.0,
                          dMaleLambda = 0.1,
                          
                          dMutRate = 0.01,
                          dMutBias = 0.0,
                          dMutEffect = 0.06,
                          
                          dSurvForage = 0.98,
                          dSurvBrood = 0.99,
                          dForagingMean = 0.5,
                          dForagingSD = 0.2,
                          const_sex_ratio = 0.25,
                          params_to_record = "dBroodingTime") {
  
  newini <- list()
  newini[["params"]] <-
    list("dLarvaMatureSize" = dLarvaMatureSize,
         
         "dForagingTime" = dForagingTime,
         "dBroodingTime" = dBroodingTime,
         "dDeathTime" = dDeathTime,
         "dMaleLambda" = dMaleLambda,
         
         "dMutRate" = dMutRate,
         "dMutBias" = dMutBias,
         "dMutEffect" = dMutEffect,
         
         "dSurvForage" = dSurvForage,
         "dSurvBrood" = dSurvBrood,
         "dForagingMean" = dForagingMean,
         "dForagingSD" = dForagingSD,
         "const_sex_ratio" = const_sex_ratio,
         "params_to_record" = params_to_record)
  
  ini::write.ini(newini, config_file_name)
}

create_config(config_file_name = "config.ini")

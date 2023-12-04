options(scipen = 999)

create_config <- function(config_file_name = "config.ini",
                          dLarvaIntercept = 5.0,       
                          dLarvaSlope = -5.0,
                          
                          dForagingTime = 3.0,
                          dBroodingTime = 4.0,
                          dLambdaForaging = 1.0,
                          dLambdaBrooding = 1.0,
                          dDeathTime = 2.0,
                          dMaleLambda = 0.1,
                          
                          dMutRate = 1.0,
                          dMutBias = 0.0,
                          dMutEffect = 0.02,
                          dSurvForage = 0.95,
                          dSurvBrood = 0.95,
                          dForagingMean = 1.5,
                          dForagingSD = 0.5,
                          dMaturingSize = 1.0,
                          const_sex_ratio = 0.3,
                          params_to_record = "dMaturingSize") {
  
  newini <- list()
  newini[["params"]] <-
    list("dLarvaIntercept" = dLarvaIntercept,
         "dLarvaSlope" = dLarvaSlope,
         "dForagingTime" = dForagingTime,
         "dBroodingTime" = dBroodingTime,
         "dLambdaForaging" = dLambdaForaging,
         "dLambdaBrooding" = dLambdaBrooding,
         "dDeathTime" = dDeathTime,
         "dMaleLambda" = dMaleLambda,
         
         "dMutRate" = dMutRate,
         "dMutBias" = dMutBias,
         "dMutEffect" = dMutEffect,
         "dSurvForage" = dSurvForage,
         "dSurvBrood" = dSurvBrood,
         "dForagingMean" = dForagingMean,
         "dForagingSD" = dForagingSD,
         "dMaturingSize" = dMaturingSize,
         "const_sex_ratio" = const_sex_ratio,
         "params_to_record" = params_to_record)
  
  ini::write.ini(newini, config_file_name)
}

create_config(config_file_name = "config.ini")

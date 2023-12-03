//
//  Parameters.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#ifndef Parameters_hpp
#define Parameters_hpp

#include "config_parser.h"
#include <array>
#include <string>
#include <vector>
#include <algorithm>

double gtime = 0.0;                 // Global time
int check_dead_males = 1000.0;      // Every check_dead_males the dead males are removed
const unsigned int max_nests = 5;      // Maximum number of nests
int max_events = 20000;             // max number of events to go through

double dInitDispersal = 0.2;        // Initial value of the dispersal gene
// Logistic expression looks like 1/(1+exp(a+b*x))
double dInitChoiceIntercept = 5.0;  // Initial value of parameter a in task choice logistic expression
double dInitChoiceSlope = -1.0;      // Initial value of parameter b in task choice logistic expression

struct params {

    params() {};

    params(const std::string& file_name) {
        read_parameters_from_ini(file_name);
    }
    
    // Below we will have some basic parameters to build the code
    double dLarvaIntercept = 5.0;       // value of parameter a in larval hatching logistic expression
    double dLarvaSlope = -5.0;           // value of parameter b in larval hatching logistic expression

    double dForagingTime = 3.0;         // Time it takes for foraging
    double dBroodingTime = 4.0;         // Time it takes for brooding
    double dLambdaForaging = 1.0;       // Poisson's constant for foraging
    double dLambdaBrooding = 1.0;       // Poisson's constant for brooding
    double dDeathTime = 2.0;
    double dMaleLambda = 10.0;          // Average lifespan of male

    double dMutRate = 1.0;              // Mutation probability
    double dMutBias = 0.0;              // Mutation Bias
    double dMutEffect = 0.02;           // Mutation effect
    double dSurvForage = 0.9;             // Survival probability at each time step for all bees
    double dSurvBrood = 0.9;
    double dForagingMean = 1.5;         // Mean of normal foraging curve
    double dForagingSD = 0.5;           // SD of normal foraging curve
    double dMaturingSize = 1.0;         // Maturing size of larvae
    double const_sex_ratio = 0.1;       // male sex ratio
    
    std::string temp_params_to_record;
    std::vector < std::string > param_names_to_record;
    std::vector < float > params_to_record;
    
    void read_parameters_from_ini(const std::string& file_name) {
        ConfigFile from_config(file_name);

        dLarvaIntercept               = from_config.getValueOfKey<double>("dLarvaIntercept");
        dLarvaSlope                   = from_config.getValueOfKey<double>("dLarvaSlope");
        dForagingTime                 = from_config.getValueOfKey<double>("dForagingTime");
        dBroodingTime                 = from_config.getValueOfKey<double>("dBroodingTime");
        dLambdaForaging               = from_config.getValueOfKey<double>("dLambdaForaging");
        dLambdaBrooding               = from_config.getValueOfKey<double>("dLambdaBrooding");
        dDeathTime                    = from_config.getValueOfKey<double>("dDeathTime");
        dMaleLambda                   = from_config.getValueOfKey<double>("dMaleLambda");
        dMutRate                      = from_config.getValueOfKey<double>("dMutRate");
        dMutBias                      = from_config.getValueOfKey<double>("dMutBias");
        dMutEffect                    = from_config.getValueOfKey<double>("dMutEffect");
        dSurvForage                   = from_config.getValueOfKey<double>("dSurvForage");
        dSurvBrood                    = from_config.getValueOfKey<double>("dSurvBrood");
        dForagingMean                 = from_config.getValueOfKey<double>("dForagingMean");
        dForagingSD                   = from_config.getValueOfKey<double>("dForagingSD");
        dMaturingSize                 = from_config.getValueOfKey<double>("dMaturingSize");
        const_sex_ratio               = from_config.getValueOfKey<double>("const_sex_ratio");
        temp_params_to_record         = from_config.getValueOfKey<std::string>("params_to_record");
        param_names_to_record         = split(temp_params_to_record);
        params_to_record              = create_params_to_record(param_names_to_record);
    }
    
    std::vector< std::string > split(std::string s) {
      // code from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
      std::vector< std::string > output;
      std::string delimiter = ",";
      size_t pos = 0;
      std::string token;
      while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        output.push_back(token);
        s.erase(0, pos + delimiter.length());
      }
      output.push_back(s);
      return output;
    }

    std::vector< float > create_params_to_record(const std::vector< std::string >& param_names) {
      std::vector< float > output;
      for (auto i : param_names) {
        output.push_back(get_val(i));
      }
      return output;
    }
    
    float get_val(std::string s) {
        if (s == "dLarvaIntercept")               return dLarvaIntercept;
        if (s == "dLarvaSlope")                   return dLarvaSlope;
        if (s == "dForagingTime")                 return dForagingTime;
        if (s == "dBroodingTime")                 return dBroodingTime;
        if (s == "dLambdaForaging")               return dLambdaForaging;
        if (s == "dLambdaBrooding")               return dLambdaBrooding;
        if (s == "dDeathTime")                    return dDeathTime;
        if (s == "dMaleLambda")                   return dMaleLambda;
        if (s == "dMutRate")                      return dMutRate;
        if (s == "dMutBias")                      return dMutBias;
        if (s == "dMutEffect")                    return dMutEffect;
        if (s == "dSurvForage")                   return dSurvForage;
        if (s == "dSurvBrood")                    return dSurvBrood;
        if (s == "dForagingMean")                 return dForagingMean;
        if (s == "dForagingSD")                   return dForagingSD;
        if (s == "dMaturingSize")                 return dMaturingSize;
        if (s == "const_sex_ratio")               return const_sex_ratio;
        
        throw std::runtime_error("can not find parameter");
        return -1.f; // FAIL
    }
};



#endif /* Parameters_hpp */

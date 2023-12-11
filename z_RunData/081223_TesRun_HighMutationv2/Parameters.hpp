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

double gtime = 0.0;                     // Global time
double max_gtime_evolution = 300000.0;     // 100000.0 
double max_gtime_LastOfUs = 5000.0;  // 500;              // Maximum for last of us generation
int check_dead_males = 5000.0;          // Every check_dead_males events the dead males are removed
const unsigned int max_nests = 1000;    // 1000 // Maximum number of nests

double dInitDispersal = -5.0;          // Initial value of the dispersal gene
// Logistic expression looks like 1/(1+exp(a+b*x))
double dInitChoiceIntercept = 0.0;    // Initial value of parameter a in task choice logistic expression
double dInitChoiceSlope = 0.0;       // Initial value of parameter b in task choice logistic expression

struct params {

    params() {};

    params(const std::string& file_name) {
        read_parameters_from_ini(file_name);
    }
    
    // Below we will have some basic parameters to build the code
    double dLarvaIntercept = 5.0;       // value of parameter a in larval hatching logistic expression
    double dLarvaSlope = -5.0;          // value of parameter b in larval hatching logistic expression

    double dForagingTime = 3.0;         // Time it takes for foraging
    double dBroodingTime = 4.0;         // Time it takes for brooding
    double dDeathTime = 2.0;            // Time it takes for death
    // LCIP: why does this take time? -> Cause they may die in the middle of their task
    // so initially we wanted to keep this a uniform distribution maybe?
    double dMaleLambda = 0.1;          // 1/Average lifespan of male

    double dMutRate = 0.01;              // Mutation probability
    double dMutBias = 0.0;              // Mutation Bias
    double dMutEffect = 0.06;           // Mutation effect

    double dSurvForage = 0.98;           // Survival probability for foraging of all bees
    double dSurvBrood = 0.99;            // Survival probability for reproduction of all bees
    double dForagingMean = 0.5;         // Mean of normal foraging curve
    double dForagingSD = 0.2;           // SD of normal foraging curve
    double const_sex_ratio = 0.5;       // male sex ratio
    
    std::string temp_params_to_record;
    std::vector < std::string > param_names_to_record;
    std::vector < float > params_to_record;
    
    void read_parameters_from_ini(const std::string& file_name) {
        ConfigFile from_config(file_name);

        dLarvaIntercept               = from_config.getValueOfKey<double>("dLarvaIntercept");
        dLarvaSlope                   = from_config.getValueOfKey<double>("dLarvaSlope");
        dForagingTime                 = from_config.getValueOfKey<double>("dForagingTime");
        dBroodingTime                 = from_config.getValueOfKey<double>("dBroodingTime");
        dDeathTime                    = from_config.getValueOfKey<double>("dDeathTime");
        dMaleLambda                   = from_config.getValueOfKey<double>("dMaleLambda");
        dMutRate                      = from_config.getValueOfKey<double>("dMutRate");
        dMutBias                      = from_config.getValueOfKey<double>("dMutBias");
        dMutEffect                    = from_config.getValueOfKey<double>("dMutEffect");
        dSurvForage                   = from_config.getValueOfKey<double>("dSurvForage");
        dSurvBrood                    = from_config.getValueOfKey<double>("dSurvBrood");
        dForagingMean                 = from_config.getValueOfKey<double>("dForagingMean");
        dForagingSD                   = from_config.getValueOfKey<double>("dForagingSD");
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
        if (s == "dDeathTime")                    return dDeathTime;
        if (s == "dMaleLambda")                   return dMaleLambda;

        if (s == "dMutRate")                      return dMutRate;
        if (s == "dMutBias")                      return dMutBias;
        if (s == "dMutEffect")                    return dMutEffect;

        if (s == "dSurvForage")                   return dSurvForage;
        if (s == "dSurvBrood")                    return dSurvBrood;
        if (s == "dForagingMean")                 return dForagingMean;
        if (s == "dForagingSD")                   return dForagingSD;
        if (s == "const_sex_ratio")               return const_sex_ratio;
        // ADD PARAMS TO RECORD
        throw std::runtime_error("can not find parameter");
        return -1.f; // FAIL
    }
};

#endif /* Parameters_hpp */

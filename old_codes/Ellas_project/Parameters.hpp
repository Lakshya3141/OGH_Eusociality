//
//  Parameters.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 10/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Parameters_hpp
#define Parameters_hpp

#include "config_params.h"
#include <vector>

// need to be initialised at compile time and thus cannot be read from the parameter file
const int max_num_b1_daughters = 9;   // maximum number of daughters produced in b1 (to not overshoot basis matrix)
double sa_spring = 0.0; // initial sex allocations gene value for b1 (fixed gene value if no sa evolution) - logistically transformed into actual sex allocations
double sa_summer = 0.0; // initial sex allocations gene value for b2 (fixed gene value if no sa evolution) - logistically transformed into actual sex allocations
const int num_knots_disp = 8; // number of knots for dispersal spline function
const int num_knots_off = 8; // number of knots for offspring spline functions
const int resolution = 100;
const double max_bodysize = 5.0;
const double no_manip_av_bodysize = 3.0; // average bodysize of offspring in no manipulation simulations (and initial body size for all simulations)
double init_disp_int = 3.5; // initial intercept of dispersal spline (logistically transformed into dispersal probability)

struct params {
    params() {};
    
    params(const std::string& file_name) {
        read_parameters_from_ini(file_name);
    }
    
    // general parameters
    int replicate = 1;  // replicate number changed in job array
    int years = 10000;             // how long simulation is run for
    int output_interval_summary = 50;    // data output interval for summary data
    int output_interval_genes = 100;  // data output interval for genes (shouldn't be too frequent)
    //int final_years = 1000; // number of years at end of simulation to test for stationarity
    int max_b1_nests = 1000;      // maximum number of nests in brood 1
    double no_manip_sd_bodysize = 0.2; // sd of average bodysize of daughters in no manipulation simulations
    
    // simulation scenarios
    bool nest_inheritance = true; // helper inherit nest from dead queen = true; helpers die when queen dies = false
    int years_no_disp_evol = 1000; // number of years that population doesn't evolve dispersal rns from initialisation 
    int years_no_manip = 10000; // number of years that maternal manipulation does not occur & manip genes don't evolve
    int years_no_sa_evol = 0; // number of years that sa ratios do not evolve from initialisation
    double pop_av_matings = 1.0; // population average number of matings per female (stochastically rounded per female for actual number of matings)
    
    // mutation parameters
    double mut_rate = 0.05;     // probability that an individual mutates
    double mut_bias = 0.0;   // mutational bias
    double mut_effect = 0.001; // mutation effect size
    
    // resource acquisition parameters (body size v resources function) for helpers and breeders
    double rec_sl_n_helper = 2.0; // slope in numerator of function
    double rec_int_helper = 0.4; // intercept in denominator of function
    double rec_sl_d_helper = 0.3; // slope in denominator of function
    double rec_min_bodysize_helper = 0.2; // minimum body size required to gain resources from foraging
    double rec_sl_n_breeder = 2.0; // slope in numerator of function
    double rec_int_breeder = 0.4; // intercept in denominator of function
    double rec_sl_d_breeder = 0.3; // slope in denominator of function
    double rec_min_bodysize_breeder = 0.2; // minimum body size required to gain resources from foraging
    
    // survival parameters (independent of body size)
    double survival_prob_m = 0.7; // probability that a brood 1 male survives to brood 2
    double survival_prob_f = 0.8; // probability that a brood 1 female survives to brood 2

    // survival parameters (dependent on body size)
    double surv_b1b2_slope = -5.0;  // slope for logistic function determining survival from b1 to b2
    double surv_b1b2_intercept = 2.5; // intercept for logistic function determining survival from b1 to b2
    double surv_diapause_slope = -5.0; // slope for logistic function determining survival through diapause
    double surv_diapause_intercept = 2.5; // intercept for logistic function determining survival through diapause

    std::string temp_params_to_record;
    std::vector<std::string> param_names_to_record;
    std::vector<float> params_to_record;
    
    void read_parameters_from_ini(const std::string& file_name) {
        ConfigFile from_config(file_name);
        
        replicate = from_config.getValueOfKey<int>("replicate");
        years = from_config.getValueOfKey<int>("years");
        output_interval_summary = from_config.getValueOfKey<int>("output_interval_summary");
        output_interval_genes = from_config.getValueOfKey<int>("output_interval_genes");
        max_b1_nests = from_config.getValueOfKey<int>("max_b1_nests");
        no_manip_sd_bodysize = from_config.getValueOfKey<double>("no_manip_sd_bodysize");
        nest_inheritance = from_config.getValueOfKey<bool>("nest_inheritance");
        years_no_disp_evol = from_config.getValueOfKey<int>("years_no_disp_evol");
        years_no_manip = from_config.getValueOfKey<int>("years_no_manip");
        years_no_sa_evol = from_config.getValueOfKey<int>("years_no_sa_evol");
        pop_av_matings = from_config.getValueOfKey<double>("pop_av_matings");
        mut_rate = from_config.getValueOfKey<double>("mut_rate");
        mut_bias = from_config.getValueOfKey<double>("mut_bias");
        mut_effect = from_config.getValueOfKey<double>("mut_effect");
        rec_sl_n_helper = from_config.getValueOfKey<double>("rec_sl_n_helper");
        rec_int_helper = from_config.getValueOfKey<double>("rec_int_helper");
        rec_sl_d_helper = from_config.getValueOfKey<double>("rec_sl_d_helper");
        rec_min_bodysize_helper = from_config.getValueOfKey<double>("rec_min_bodysize_helper");
        rec_sl_n_breeder = from_config.getValueOfKey<double>("rec_sl_n_breeder");
        rec_int_breeder = from_config.getValueOfKey<double>("rec_int_breeder");
        rec_sl_d_breeder = from_config.getValueOfKey<double>("rec_sl_d_breeder");
        rec_min_bodysize_breeder = from_config.getValueOfKey<double>("rec_min_bodysize_breeder");
        survival_prob_m = from_config.getValueOfKey<double>("survival_prob_m");
        survival_prob_f = from_config.getValueOfKey<double>("survival_prob_f");
        surv_b1b2_slope = from_config.getValueOfKey<double>("surv_b1b2_slope");
        surv_b1b2_intercept = from_config.getValueOfKey<double>("surv_b1b2_intercept");
        surv_diapause_slope = from_config.getValueOfKey<double>("surv_diapause_slope");
        surv_diapause_intercept = from_config.getValueOfKey<double>("surv_diapause_intercept");
        temp_params_to_record = from_config.getValueOfKey<std::string>("params_to_record");
        param_names_to_record = split(temp_params_to_record);
        params_to_record = create_params_to_record(param_names_to_record);
    }
    
    std::vector<std::string> split(std::string s) {
    // code from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
        std::vector<std::string> output;
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
    
    std::vector<float> create_params_to_record(const std::vector<std::string>& param_names) {
        std::vector<float> output;
        for (auto i : param_names) {
            output.push_back(get_val(i));
        }
        return output;
    }
    
    float get_val(std::string s) {
        if(s == "replicate") return replicate;
        if(s == "years") return years;
        if(s == "output_interval_summary") return output_interval_summary;
        if(s == "output_interval_genes") return output_interval_genes;
        if(s == "max_b1_nests") return max_b1_nests;
        if(s == "no_manip_sd_bodysize") return no_manip_sd_bodysize;
        if(s == "nest_inheritance") return nest_inheritance;
        if(s == "years_no_disp_evol") return years_no_disp_evol;
        if(s == "years_no_manip") return years_no_manip;
        if(s == "years_no_sa_evol") return years_no_sa_evol;
        if(s == "pop_av_matings") return pop_av_matings;
        if(s == "mut_rate") return mut_rate;
        if(s == "mut_bias") return mut_bias;
        if(s == "mut_effect") return mut_effect;
        if(s == "rec_sl_n_helper") return rec_sl_n_helper;
        if(s == "rec_int_helper") return rec_int_helper;
        if(s == "rec_sl_d_helper") return rec_sl_d_helper;
        if(s == "rec_min_bodysize_helper") return rec_min_bodysize_helper;
        if(s == "rec_sl_n_breeder") return rec_sl_n_breeder;
        if(s == "rec_int_breeder") return rec_int_breeder;
        if(s == "rec_sl_d_breeder") return rec_sl_d_breeder;
        if(s == "rec_min_bodysize_breeder") return rec_min_bodysize_breeder;
        if(s == "survival_prob_m") return survival_prob_m;
        if(s == "survival_prob_f") return survival_prob_f;
        if(s == "surv_b1b2_slope") return surv_b1b2_slope;
        if(s == "surv_b1b2_intercept") return surv_b1b2_intercept;
        if(s == "surv_diapause_slope") return surv_diapause_slope;
        if(s == "surv_diapause_intercept") return surv_diapause_intercept;
        
        throw std::runtime_error("can not find parameter");
        return -1.f; // FAIL
    }
};


#endif /* Parameters_hpp */


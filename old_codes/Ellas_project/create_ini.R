options(scipen = 999) #turn off scientific notation

# create config file function
# params_to_record = parameter types for which different values are investigated in different simulations
create_config <- function(config_file_name = "config.ini",
                          years = 50000,
                          output_interval_summary = 10,
                          output_interval_genes = 5000,
                          max_b1_nests = 1000,
                          no_manip_sd_bodysize = 0.2,
                          maternal_manipulation = 1,
                          nest_inheritance = 0,
                          sex_ratio_evolution = 0,
                          years_solitary = 0,
                          pop_av_matings = 1.0,
                          mut_rate = 0.03,
                          mut_bias = 0.0,
                          mut_effect = 0.03,
                          mut_effect_disp_int = 0.12,
                          rec_sl_n = 4.0,
                          rec_int = 0.4,
                          rec_sl_d = 0.4,
                          rec_min_bodysize = 0.5,
                          survival_prob_m = 0.8,
                          survival_prob_f = 0.9,
                          surv_b1b2_slope = -8.0,
                          surv_b1b2_intercept = 6.0,
                          surv_diapause_slope = -6.6,
                          surv_diapause_intercept = 7.5,
                          params_to_record = "rec_min_bodysize") {
  newini <- list()
  newini[["params"]] <- list("years" = years,
                             "output_interval_summary" = output_interval_summary,
                             "output_interval_genes" = output_interval_genes,
                             "max_b1_nests" = max_b1_nests,
                             "no_manip_sd_bodysize" = no_manip_sd_bodysize,
                             "maternal_manipulation" = maternal_manipulation,
                             "nest_inheritance" = nest_inheritance,
                             "sex_ratio_evolution" = sex_ratio_evolution,
                             "years_solitary" = years_solitary,
                             "pop_av_matings" = pop_av_matings,
                             "mut_rate" = mut_rate,
                             "mut_bias" = mut_bias,
                             "mut_effect" = mut_effect,
                             "mut_effect_disp_int" = mut_effect_disp_int,
                             "rec_sl_n" = rec_sl_n,
                             "rec_int" = rec_int,
                             "rec_sl_d" = rec_sl_d,
                             "rec_min_bodysize" = rec_min_bodysize,
                             "survival_prob_m" = survival_prob_m,
                             "survival_prob_f" = survival_prob_f,
                             "surv_b1b2_slope" = surv_b1b2_slope,
                             "surv_b1b2_intercept" = surv_b1b2_intercept,
                             "surv_diapause_slope" = surv_diapause_slope,
                             "surv_diapause_intercept" = surv_diapause_intercept,
                             "params_to_record" = params_to_record)
  
  ini::write.ini(newini, config_file_name)
}

create_config(config_file_name = "config.ini")


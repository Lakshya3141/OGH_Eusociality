//
//  Population.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 13/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Population_hpp
#define Population_hpp

#include "Nest.hpp"
#include <iomanip>

class Population {
public:
    Population() = default;
    
    std::vector<Nest> nests; // vector of nests
    std::vector<Individual<1>> males; // global vector of males to mate with females
    
    void initialise_pop(const params& par);
    void survival_males(const params& p);
    void first_brood(const params& p, const int year);
    void second_brood(const params& p, const int year);
    void pop_size_control(const params& p);
    void evaluate_survival(const bool is_first_brood, const params& p);
    void remove_empty_nests(const params& p);
    void mate_offspring(const params& p, std::vector<Nest>& new_daughters);
    void create_sons(const params& p, double& m_resources, int& surv_sons, const int& year, diploid_genome& genome_mum);
    
    Eigen::MatrixXd basis_matrix_dispersal {make_spline2d(num_knots_disp, resolution, 0.0, max_bodysize)};
    Eigen::MatrixXd basis_matrix_offspring {make_spline2d(num_knots_off, resolution, 0.0, max_num_b1_daughters)};
    
    // for data output
    void write_headers(const std::vector<std::string>& param_names, const params& p);
    void data_analysis_summary(const int year, const bool is_first_brood, const std::vector<float>& param_values);
    std::ofstream results_summary;
    void data_analysis_genes(const int year, const bool is_first_brood, const std::vector<float>& param_values);
    std::ofstream results_genes;
    void write_stationarity_data();
    std::ofstream output_stationarity;
    void write_basis_matrix_dispersal();
    std::ofstream output_basis_matrix_dispersal;
    void write_basis_matrix_offspring();
    std::ofstream output_basis_matrix_offspring;
    
    params p;
};

// initialise population function
void Population::initialise_pop(const params& par) {

    p = par;
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    Individual<2> eve;
    eve.calculate_phenotype();
    Individual<1> adam;
    
    sperm_genomes initial_mates; // temporary vector for sperm genome
    initial_mates.push_back(adam.genome);
    
    eve.mate(initial_mates); // female mated with adam
    eve.body_size = no_manip_av_bodysize; // initialise body size
    for(int i=0; i<p.max_b1_nests; ++i) {
        nests.emplace_back(eve);  // loops through each nest and adds mated founding female
    }
}

// writes headers of output csv files
void Population::write_headers(const std::vector<std::string>& param_names, const params& p) {
    
    // prepare output files
    std::ostringstream str;
    
    // file name
    str << "data_summary"
        << "_years_no_manip_" << p.years_no_manip
        << "_num_mates_" << p.pop_av_matings
        << "_rep_" << p.replicate
        << ".csv";
    results_summary.open(str.str()); // open file and start writing
    
    for(auto& i : param_names) {
        results_summary << i << ',';
    }
        
    results_summary << "year" << ','
                    << "brood" << ','
                    << "number_males" << ','
                    << "number_nests" << ','
                    << "number_sol_nests" << ','
                    << "number_soc_nests" << ','
                    << "prop_sol_nests" << ','
                    << "number_females" << ','
                    << "num_dispersers" << ','
                    << "av_male_bodysize" << ','
                    << "av_queen_bodysize" << ','
                    << "av_queen_bodysize_dispersers" << ','
                    << "av_queen_bodysize_survivors" << ','
                    << "av_helper_bodysize" << ','
                    << "av_disp_int" << ',';
    for (int i=1; i<num_knots_disp; ++i) {
        results_summary << "av_disp_sl" << i << ',';
    }
    
    results_summary << "av_off_b1_int" << ',';
    for (int i=1; i<num_knots_off; ++i) {
        results_summary << "av_off_b1_sl" << i << ',';
    }
    
    results_summary << "av_queen_sa_b1" << ','
                    << "av_queen_sa_b2" << ','
                    << "av_num_offspring" << ','
                    << "av_num_offspring_sol" << ','
                    << "av_num_offspring_soc" << ','
                    << "av_nest_resources" << ','
                    << "av_nest_resources_sol" << ','
                    << "av_nest_resources_soc" << ','
                    << "av_nest_fem_off_bodysize" << ','
                    << "av_nest_realised_sr"
                    << "\n";
    
    std::ostringstream str_genes;
    
    // file name
    str_genes << "data_genes"
              << "_years_no_manip_" << p.years_no_manip
              << "_num_mates_" << p.pop_av_matings
              << "_rep_" << p.replicate
              << ".csv";
    results_genes.open(str_genes.str()); // open file and start writing
    
    for(auto& i : param_names) {
        results_genes << i << ',';
    }
    
    results_genes << "year" << ','
                  << "brood" << ','
                  << "is_helper" << ','
                  << "is_disperser" << ','
                  << "body_size" << ','
                  << "genes_disp_int" << ',';
    
    for (int i=1; i<num_knots_disp; ++i) {
        results_genes << "genes_disp_sl" << i << ',';
    }
    
    results_genes << "genes_off_b1_int" << ',';
    
    for (int i=1; i<num_knots_off; ++i) {
        results_genes << "genes_off_b1_sl" << i << ',';
    }
    results_genes << "genes_sa_b1" << ','
                  << "genes_sa_b2"
                  << "\n";
    
    std::ostringstream str_basis_matrix_dispersal;
    str_basis_matrix_dispersal << "basis_matrix_dispersal.csv"; // file name
    output_basis_matrix_dispersal.open(str_basis_matrix_dispersal.str()); // open file and start writing
    
    std::ostringstream str_basis_matrix_offspring;
    str_basis_matrix_offspring << "basis_matrix_offspring.csv"; // file name
    output_basis_matrix_offspring.open(str_basis_matrix_offspring.str()); // open file and start writing
    
    std::ostringstream str_stationarity;
    str_stationarity << "stationarity.csv"; // file name
    output_stationarity.open(str_stationarity.str()); // open file and start writing
    
}

// outputs the basis matrix of the dispersal spline
void Population::write_basis_matrix_dispersal() {
    
    for (int i = 0; i < basis_matrix_dispersal.rows(); ++i) {
        for (int j = 0; j < basis_matrix_dispersal.cols(); ++j) {
            output_basis_matrix_dispersal << basis_matrix_dispersal(i, j) << ',';
        }
        output_basis_matrix_dispersal << '\n';
    }
    output_basis_matrix_dispersal.flush();
}

// outputs the basis matrix of the offspring manipulation spline
void Population::write_basis_matrix_offspring() {
    
    for (int i = 0; i < basis_matrix_offspring.rows(); ++i) {
        for (int j = 0; j < basis_matrix_offspring.cols(); ++j) {
            output_basis_matrix_offspring << basis_matrix_offspring(i, j) << ',';
        }
        output_basis_matrix_offspring << '\n';
    }
    output_basis_matrix_offspring.flush();
}

// data analysis function for summary
void Population::data_analysis_summary(const int year, const bool is_first_brood, const std::vector<float>& param_values) {
    
    // write parameter values to file
    for(auto& i : param_values) {
        results_summary << i << ',';
    }
    
    int num_females = 0, nest_counter = 0, num_solitary = 0, num_social = 0, num_dispersers = 0, helper_counter = 0, num_survivors = 0;
    double sum_male_bodysize = 0.0, sum_bodysize_helpers = 0.0, sum_bodysize_queens = 0.0, sum_bodysize_queens_dispersers = 0.0, sum_bodysize_queens_survivors = 0.0, sum_sa_b1 = 0.0, sum_sa_b2 = 0.0;
    std::array<double, num_knots_off> sum_off_genes_b1 = {0.0}, av_off_genes_b1 = {0.0};
    std::array<double, num_knots_disp> sum_disp_genes = {0.0}, av_disp_genes = {0.0};
    
    for (auto& nest : nests) {
        
        for(auto& f : nest.females) {
            ++num_females;
            
            for (int i=0; i<num_knots_disp; ++i) {
                sum_disp_genes[i] += f.phenotype_dispersal(i, 0);
            }
        }
        
        nest.check_solitary();
        
        // for solitary nests:
        if(nest.is_solitary) {
            ++num_solitary;
            if(nest.females[0].disperser) {
                ++num_dispersers;
                sum_bodysize_queens_dispersers += nest.females[0].body_size;
            }
            else {
                ++num_survivors;
                sum_bodysize_queens_survivors += nest.females[0].body_size;
            }
        }
        
        // for social nests:
        else {
            ++num_social;
            
            // output values for helpers:
            for(size_t i=1; i<nest.females.size(); ++i) {
                sum_bodysize_helpers += nest.females[i].body_size;
                ++helper_counter;
            }
            
            ++num_survivors;
            sum_bodysize_queens_survivors += nest.females[0].body_size;
        }
        
        // output values for foundresses:
        sum_bodysize_queens += nest.females[0].body_size;
        
        for (int i=0; i<num_knots_off; ++i) {
            sum_off_genes_b1[i] += nest.females[0].phenotype_offspring_b1(i, 0);
        }
        
        sum_sa_b1 += nest.females[0].phenotype_sex_allocations[0];
        sum_sa_b2 += nest.females[0].phenotype_sex_allocations[1];
        
        ++nest_counter;
    }
    
    // calculate averages across nests
    double prop_sol_nests = (double)num_solitary / (double)nest_counter;        // proportion of solitary nests
    double av_helper_bodysize = sum_bodysize_helpers / (double)helper_counter;      // average body size of helpers across all nests
    double av_queen_bodysize = sum_bodysize_queens / (double)nest_counter;          // average body size of queens across all nests (where queen = females[0])
    double av_queen_bodysize_dispersers = sum_bodysize_queens_dispersers / (double)num_dispersers;      // average body size of dispersed queens across all nests
    double av_queen_bodysize_survivors = sum_bodysize_queens_survivors / (double)num_survivors;      // average body size of surviving b1 to b2 queens across all nests
    
    for (int i=0; i<num_knots_disp; ++i) {
        av_disp_genes[i] = sum_disp_genes[i] / (double)num_females;
    }
    
    for (int i=0; i<num_knots_off; ++i) {
        av_off_genes_b1[i] = sum_off_genes_b1[i] / (double)nest_counter;
    }

    double av_sa_b1 = sum_sa_b1 / (double)nest_counter;     // average queen b1 sa phenotype value across all nests
    double av_sa_b2 = sum_sa_b2 / (double)nest_counter;     // average queen b2 sa phenotype value across all nests
    
    for (size_t i=0; i<males.size(); ++i) {
        sum_male_bodysize += males[i].body_size;
    }
    double av_male_bodysize = sum_male_bodysize / (double)males.size();
    
    // write output
    results_summary << year << ',';
    
    if(is_first_brood == true) results_summary << "1" << ',';
    else results_summary << "2" << ',';
    
    results_summary << males.size() << ','
                    << std::fixed << nests.size() << ','
                    << num_solitary << ','
                    << num_social << ','
                    << std::setprecision(3) << prop_sol_nests << ','
                    << num_females << ','
                    << num_dispersers << ','
                    << av_male_bodysize << ','
                    << av_queen_bodysize << ','
                    << av_queen_bodysize_dispersers << ','
                    << av_queen_bodysize_survivors << ','
                    << av_helper_bodysize << ',';
    
    for (int i=0; i<num_knots_disp; ++i) {
        results_summary << av_disp_genes[i] << ',';
    }
    
    for (int i=0; i<num_knots_off; ++i) {
        results_summary << av_off_genes_b1[i] << ',';
    }
    
    results_summary << av_sa_b1 << ','
                    << av_sa_b2 << ',';
    
    results_summary.flush();
}

// data analysis function for individual values like genes
void Population::data_analysis_genes(const int year, const bool is_first_brood, const std::vector<float>& param_values) {
    
    // write output
    for(size_t i=0; i<nests.size(); ++i) {
        for(size_t j=0; j<nests[i].females.size(); ++j) {
            
            // write parameter values to file
            for(auto k : param_values) {
                results_genes << k << ',';
            }
            
            results_genes << year << ',';
            if(is_first_brood) results_genes << "1" << ',';
            else results_genes << "2" << ',';
            results_genes << std::boolalpha << nests[i].females[j].helper << ','
                          << std::boolalpha << nests[i].females[j].disperser << ','
                          << nests[i].females[j].body_size << ',';
            
            for (int k=0; k< num_knots_disp; ++k) {
                results_genes << nests[i].females[j].phenotype_dispersal(k,0) << ',';
            }
            
            for (int k=0; k< num_knots_off; ++k) {
                results_genes << nests[i].females[j].phenotype_offspring_b1(k,0) << ',';
            }
            
            results_genes << nests[i].females[j].phenotype_sex_allocations[0] << ','
                          << nests[i].females[j].phenotype_sex_allocations[1]
                          << '\n';
        }
    }
    results_genes.flush();
}

void Population::write_stationarity_data() {
    
    int num_soc = 0, nest_counter = 0;
    
    for(auto& nest : nests) {
        ++nest_counter;
        nest.check_solitary();
        
        if(!nest.is_solitary) {
            ++num_soc;
        }
    }
    
    double prop_soc = (double)num_soc / (double)nest_counter;
    
    output_stationarity << prop_soc << '\n';
}


// SPRING BROOD
void Population::first_brood(const params& p, const int year) {
    
    std::vector<Nest> new_nests; // temporary vector for female offspring
    new_nests.reserve(max_num_b1_daughters * p.max_b1_nests); // reserve space for vector

    int sum_num_offspring = 0; // for data output
    double sum_resources = 0.0, sum_nest_av_fem_off_bodysize = 0.0, sum_realised_sr = 0.0;
    
    for(size_t i=0; i<nests.size(); ++i) { // loops through nests
        
        nests[i].forage(p); // forage for resources
        sum_resources += nests[i].nest_resources;
        
        double fem_resources = nests[i].divide_resources(true); // proportion of total resources allocated to female offspring
        double male_resources = nests[i].nest_resources - fem_resources;
        
        int num_surv_sons = 0;
        
        create_sons(p, male_resources, num_surv_sons, year, nests[i].females[0].genome);
    
        double sum_fem_bodysizes = 0.0;
        int num_daughters = 0, num_surv_daughters = 0;
        
        // loop through potential daughters
        for (int j=0; j<max_num_b1_daughters; ++j) {
            
            // evaluate body size of potential daughter
            double temp_bodysize = 0.0;
            if (year <= p.years_no_manip) { // if in the potential no maternal manipulation phase
                temp_bodysize = normal(no_manip_av_bodysize, p.no_manip_sd_bodysize);
            }
            else { // if in the potential maternal manipulation phase
                temp_bodysize = evaluate_spline2d(basis_matrix_offspring, nests[i].females[0].phenotype_offspring_b1, j);
            }
            
            // body size should not exceed range 
            if (temp_bodysize < 0.0) temp_bodysize = 0.0;
            if (temp_bodysize > max_bodysize) temp_bodysize = max_bodysize;
            
            fem_resources -= temp_bodysize; // resources allocated to daughter subtracted from total
            
            // create temp daughter
            Individual<2> temp_female = Individual<2>(nests[i].females[0].genome, nests[i].females[0].sperm[uni_int(0, nests[i].females[0].sperm.size())], p, year);
            temp_female.body_size = temp_bodysize;
            
            // if there are sufficient resources, daughter is created by default
            if (fem_resources >= 0.0) {
                // is_alive = true by default
                sum_fem_bodysizes += temp_bodysize;
                ++num_daughters;
            }
            // if there are insufficient resources, probability of creating final daughter is checked
            else {
                double prob = (fem_resources + temp_bodysize) / temp_bodysize;
                if (bernoulli(prob)) {
                    // is_alive = true by default
                    sum_fem_bodysizes += temp_bodysize;
                    ++num_daughters;
                }
                else {
                    temp_female.is_alive = false;
                }
                j = max_num_b1_daughters; // to end loop, no subsequent daughters created
            }
            
            if (temp_female.is_alive) { // if the daughter was created
                
                ///double prob = logistic(temp_female.body_size, p.surv_b1b2_intercept, p.surv_b1b2_slope); // check survival of new female offspring based on body size
                ///if (bernoulli(prob)) { // if the female survives
                    ++num_surv_daughters;
                    
                    // check if female disperses based on logistictically transformed dispersal spline evaluation
                    // if female disperses
                    if (temp_female.check_dispersal(basis_matrix_dispersal)) {  // check_dispersal function will mark her as a disperser
                        new_nests.emplace_back(std::move(temp_female));  // added to new nest, becomes new breeder
                    }
                    
                    // if female does not disperse
                    else {
                        temp_female.helper = true;  // mark her as helper
                        nests[i].females.push_back(std::move(temp_female)); // add unmated to current nest
                    }
                    
                ///}
                
                // if temp_female did not survive, she is not added to other vectors and thus dies at end of loop
            }
        
        }
        
        if(num_daughters != 0) {
            sum_nest_av_fem_off_bodysize += sum_fem_bodysizes / (double)num_daughters; // per nest average female offspring body size (before survival called)
        }
        
        int num_off = num_surv_sons + num_surv_daughters;
        if (num_off != 0) {
            sum_num_offspring += num_off;
            sum_realised_sr += (double)num_surv_sons / (double)num_off;
        }
        
    } // end loop through nests
    
    
    if(year % p.output_interval_summary == 0) {
        double av_num_offspring = (double)sum_num_offspring / (double)nests.size();
        double av_num_offspring_sol = av_num_offspring;
        double av_num_offspring_soc = 0.0;
        double av_nest_resources = sum_resources / (double)nests.size();
        double av_nest_resources_sol = av_nest_resources;
        double av_nest_resources_soc = 0.0;
        double av_nest_fem_off_bodysize = sum_nest_av_fem_off_bodysize / (double)nests.size();
        double av_nest_realised_sr = sum_realised_sr / (double)nests.size();
        results_summary << av_num_offspring << ',' << av_num_offspring_sol << ',' << av_num_offspring_soc << ',' << av_nest_resources << ',' << av_nest_resources_sol << ',' << av_nest_resources_soc << ',' << av_nest_fem_off_bodysize << ',' << av_nest_realised_sr << '\n';
        results_summary.flush();
    }
    
    mate_offspring(p, new_nests); // mate males with new dispersing daughters

    evaluate_survival(true, p); // evaluate survival of foundresses to brood 2
    
    nests.insert(nests.end(), std::make_move_iterator(new_nests.begin()), std::make_move_iterator(new_nests.end())); // add mated dispersing daughters into nests vector
    
    // survival
    remove_empty_nests(p);    // dead females removed, then empty nests removed
    survival_males(p);        // remove dead males
    
}

// SUMMER BROOD
void Population::second_brood(const params& p, const int year) {
    
    std::vector<Nest> new_nests; // temporary vector for female offspring
    new_nests.reserve(max_num_b1_daughters * nests.size()); // reserve space for vector
    
    int sum_num_offspring = 0, sum_num_offspring_soc = 0, sum_num_offspring_sol = 0, num_soc_nests = 0, num_sol_nests = 0; // for data output
    double sum_resources = 0.0, sum_resources_soc = 0.0, sum_resources_sol = 0.0, sum_nest_av_fem_off_bodysize = 0.0, sum_realised_sr = 0.0;
    
    for(auto& nest : nests) { // loops through nests (incl. surviving foundresses + b1 daughters)
        
        nest.check_solitary(); // determines whether nest is solitary or social (for data output)
        
        nest.forage(p);
        sum_resources += nest.nest_resources;
        
        double fem_resources = nest.divide_resources(false); // proportion of total resources allocated to female offspring (false = brood 2)
        double male_resources = nest.nest_resources - fem_resources;
        
        int num_surv_sons = 0;
        
        create_sons(p, male_resources, num_surv_sons, year, nest.females[0].genome);
        
        double sum_fem_bodysizes = 0.0;
        int num_daughters = 0;
        
        // create daughters
        while (fem_resources > 0.0) {
            Individual<2> temp_female = Individual<2>(nest.females[0].genome, nest.females[0].sperm[uni_int(0, nest.females[0].sperm.size())], p, year);
            temp_female.body_size = normal(no_manip_av_bodysize, p.no_manip_sd_bodysize);
            temp_female.range_bodysize();
            
            fem_resources -= temp_female.body_size;
            
            // if there are still sufficient resources, female created by default
            if (fem_resources > 0.0) {
                ///double prob = logistic(temp_female.body_size, p.surv_b1b2_intercept, p.surv_b1b2_slope); // check baseline larval survival
                ///if (bernoulli(prob)) {
                    ++num_daughters;
                    sum_fem_bodysizes += temp_female.body_size;
                    new_nests.emplace_back(std::move(temp_female));
                ///}
            }
            // if there are not sufficient resources, stochastically round to see if final female created
            else {
                double prob1 = (fem_resources + temp_female.body_size) / temp_female.body_size;
                if (bernoulli(prob1)) {
                    ///double prob2 = logistic(temp_female.body_size, p.surv_b1b2_intercept, p.surv_b1b2_slope); // check baseline larval survival
                    ///if (bernoulli(prob2)) {
                        ++num_daughters;
                        sum_fem_bodysizes += temp_female.body_size;
                        new_nests.emplace_back(std::move(temp_female));
                    ///}
                }
            }
            // if temp female was not created or didn't survive, she is not added to new_nests vector and thus dies at end of while loop
        }
        
        if (num_daughters != 0) {
            sum_nest_av_fem_off_bodysize += sum_fem_bodysizes / (double)num_daughters; // per nest average female offspring body size (before diapause survival called)
        }
        
        int num_off = num_surv_sons + num_daughters;
        if (num_off != 0) {
            sum_num_offspring += num_off;
            sum_realised_sr += (double)num_surv_sons / (double)num_off;
        }
        
        if(nest.is_solitary) {
            ++num_sol_nests;
            sum_resources_sol += nest.nest_resources;
            sum_num_offspring_sol += num_off;
        }
        else {
            ++num_soc_nests;
            sum_resources_soc += nest.nest_resources;
            sum_num_offspring_soc += num_off;
        }
        
    } // end nests loop
    
    if(year % p.output_interval_summary == 0) {
        double av_num_offspring = (double)sum_num_offspring / (double)nests.size();
        double av_num_offspring_sol = (double)sum_num_offspring_sol / (double)num_sol_nests;
        double av_num_offspring_soc = (double)sum_num_offspring_soc / (double)num_soc_nests;
        double av_nest_resources = sum_resources / (double)nests.size();
        double av_nest_resources_sol = sum_resources_sol / (double)num_sol_nests;
        double av_nest_resources_soc = sum_resources_soc / (double)num_soc_nests;
        double av_nest_fem_off_bodysize = sum_nest_av_fem_off_bodysize / (double)nests.size();
        double av_nest_realised_sr = sum_realised_sr / (double)nests.size();
        results_summary << av_num_offspring << ',' << av_num_offspring_sol << ',' << av_num_offspring_soc << ',' << av_nest_resources << ',' << av_nest_resources_sol << ',' << av_nest_resources_soc << ',' << av_nest_fem_off_bodysize << ',' << av_nest_realised_sr << '\n';
        results_summary.flush();
    }
    
    mate_offspring(p, new_nests); // mate males with new daughters

    // swap temporary vector (daughters become new breeders, old females die)
    nests.swap(new_nests);
    
    // survival & pop size control
    evaluate_survival(false,p); // evaluates survival of new females through diapause to brood 1
    remove_empty_nests(p);      // dead females removed, then empty nests removed
    pop_size_control(p);        // randomly remove nests from vector to not exceed pop_size
    males.clear();              // all males die
    
}

// create sons & add to global male pool if they survive
void Population::create_sons(const params& p, double& m_resources, int& surv_sons, const int& year, diploid_genome& genome_mum) {
    while (m_resources > 0.0) {
        Individual<1> temp_male = Individual<1>(genome_mum, p, year);
        temp_male.body_size = normal(no_manip_av_bodysize, p.no_manip_sd_bodysize);
        temp_male.range_bodysize();
        
        m_resources -= temp_male.body_size;
        
        // if there are still sufficient resources, male created by default
        if (m_resources > 0.0) {
            ///double prob = logistic(temp_male.body_size, p.surv_b1b2_intercept, p.surv_b1b2_slope); // check baseline larval survival
            ///if (bernoulli(prob)) {
                males.push_back(std::move(temp_male));
                ++surv_sons;
            ///}
        }
        // if there are not sufficient resources, stochastically round to see if final male created
        else {
            double prob1 = (m_resources + temp_male.body_size) / temp_male.body_size;
            if (bernoulli(prob1)) {
                ///double prob2 = logistic(temp_male.body_size, p.surv_b1b2_intercept, p.surv_b1b2_slope); // check baseline larval survival
                ///if (bernoulli(prob2)) {
                    males.push_back(std::move(temp_male));
                    ++surv_sons;
                ///}
            }
        }
        // if temp male was not created or didn't survive, he is not added to males vector and thus dies at end of while loop
    }
}

// mate males with newly created (dispersing) daughters
void Population::mate_offspring(const params& p, std::vector<Nest>& new_daughters) {
    for(size_t i=0; i<new_daughters.size(); ++i) {     // loops through nests of new daughters
        
        if(!males.empty()) {  // if there are males
            int num_mates = stochastic_rounding(p.pop_av_matings); // determines actual number of mates a female will have
            if(males.size() < num_mates) num_mates = (int)males.size(); // ensures number of mates doesn't exceed number of males
            
            sperm_genomes temp_mates;
            for(int k=0; k<num_mates; ++k) {        // loops through matings to create sperm array
                temp_mates.push_back(males[uni_int(0, (males.size()))].genome);
            }
                
            new_daughters[i].females[0].mate(temp_mates); // each female mated with random males
        }
        else { // if there are no males to mate with
            new_daughters[i].females[0].is_alive = false; // females who do not successfully mate are removed from population
        }
        
    }
}

// ensures that number of nests in brood 1 does not exceed max number of nests
void Population::pop_size_control(const params& p) {
    while(nests.size() > p.max_b1_nests) {
        remove_from_vec(nests, uni_int(0, nests.size()));
    }
}
        
// check if males survive to next brood
void Population::survival_males(const params& p) {
    for (auto& m : males) {
        m.survival(p.survival_prob_m);
    }
    std::erase_if(males,[](auto& m){ return !m.is_alive;});
}

// sets bool is_alive for each female to true or false dependent on survival prob
void Population::evaluate_survival(const bool is_first_brood, const params& p) {
    // for first brood (spring)
    if (is_first_brood) {
        for (auto& nest : nests) {
            nest.females[0].survival(p.survival_prob_f);
        }
    }
    
    /*
    // for second brood (summer)
    else {
        for (auto& nest : nests) {
            double prob = logistic(nest.females[0].body_size, p.surv_diapause_intercept, p.surv_diapause_slope); // diapause survival probability dependent on body size
            nest.females[0].survival(prob);
        }
    }
    */
}

// removes nests if all females dead
void Population::remove_empty_nests(const params& p) {

    for (auto& nest : nests) {
        nest.remove_dead_females();  // loop through each nest and remove dead females
        
        if(!nest.females.empty() and nest.females[0].helper) {    // if first female is a helper (queen has died)
            
            if(p.nest_inheritance) {      // if there is nest inheritance
                nest.females[0].helper = false; // helper takes over as queen
                
                int num_mates = stochastic_rounding(p.pop_av_matings); // determines actual number of mates a female will have
                
                sperm_genomes temp_mates;
                
                for(int i=0; i<num_mates; ++i) {
                    temp_mates.push_back(males[uni_int(0, (males.size()))].genome);
                }
                
                nest.females[0].mate(temp_mates); // mate the new queen
            }
            else nest.females.clear(); // if there is no nest inheritance, nest dies
        }
        
        if(nest.females.empty()) nest.is_empty = true;
    }

    std::erase_if(nests,[](auto& nest) {return nest.is_empty;}); // remove nest if empty
}

#endif /* Population_hpp */

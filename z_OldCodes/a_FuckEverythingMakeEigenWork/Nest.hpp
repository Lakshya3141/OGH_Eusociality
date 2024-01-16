//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Nest_hpp
#define Nest_hpp    

#include "Individual.hpp"
#include <algorithm>

class Nest {
public:
    Nest(const Individual<2>& f, const int nid) ;
    
    // feed function that chooses random larva and feeds it in the nest
    std::tuple<bool, bool, size_t> feed(const double& mean, const double& SD);
    std::tuple<bool, bool, size_t> feed_ld(const double& mean, const double& SD);
    void reproduce(Individual<2>& female, const int id, const params& p);
    void task_check(const Eigen::MatrixXd& tmp_matrix, Individual<2>& current, const params& p);
    size_t findFemaleIndexById(const int id);
    void mate_withinNest(Individual<2>& female);
    std::tuple<bool, size_t> choose_RndmMaleLarva();
    void printIndividualEventCSV(const std::vector< float >& param_values, std::ostream& csv_file, const Individual<2>& current, const Individual<2>& recent, const unsigned long int event);

    unsigned int nest_id;
    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males
};

// < is there any larvae fed, female == true, index of larvae fed > : Structure of tuple returned from feed function
std::tuple<bool, bool, size_t> Nest::feed(const double& mean, const double& SD) {
    // confirm presence of larvae
    if ((larval_males.size() + larval_females.size()) > 0){
        // take weighted probabilities
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        double resources = normal(mean, SD);

        if (resources < 0.0) resources = 0.0;
        
        if (bernoulli(maleProbability)) {
            // take random male larva to feed
            size_t ind = uni_int(0, larval_males.size());
            larval_males[ind].body_size += resources;
            return std::make_tuple(true, false, ind);
        } else {
            // take random female larva to feed
            size_t ind = uni_int(0, larval_females.size());
            larval_females[ind].body_size += resources;
            return std::make_tuple(true, true, ind);
        }
    }
    else {
        return std::make_tuple(false, true, 0);
    }
}

//LD Feed implemented here
// < is there any larvae fed, female == true, index of larvae fed > : Structure of tuple returned from feed function
std::tuple<bool, bool, size_t> Nest::feed_ld(const double& mean, const double& SD) {
    // confirm presence of larvae
    double resources = normal(mean, SD);
    if (resources < 0.0) resources = 0.0;
    while ((larval_males.size() + larval_females.size()) > 0){
        // take weighted probabilities
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        if (bernoulli(maleProbability)) {
            // take random male larva to feed
            size_t ind = uni_int(0, larval_males.size());
            if (larval_males[ind].td_larva < gtime) {
                remove_from_vec(larval_males, ind);
            } else {
                larval_males[ind].body_size += resources;
                return std::make_tuple(true, false, ind);
            }
        } else {
            // take random female larva to feed
            size_t ind = uni_int(0, larval_females.size());
            if (larval_females[ind].td_larva < gtime) {
                remove_from_vec(larval_females, ind);
            } else {
                larval_females[ind].body_size += resources;
                return std::make_tuple(true, true, ind);
            }
        }
    }
    return std::make_tuple(false, true, 0);
}


// Nest constructor from breeder and Nest ID
Nest::Nest(const Individual<2>& f, const int nid) : nest_id(nid) {
    adult_females.push_back(f);
} // Constructor for nest



// produces males if not mated or with bernoulli(male sex ratio)
void Nest::reproduce(Individual<2>& female, const int id, const params& p) {
    if (female.is_mated) {
        // offspring sex depends on paramaters if mated
        if (bernoulli(p.const_sex_ratio)) {
            larval_males.emplace_back(id, female, p);
        } else {
            larval_females.emplace_back(id, female, p);
            female.num_female_larva++;      // increment female num larvae if female
        }
    } else {
        // offsprings are male if not mated
        larval_males.emplace_back(id, female, p);
    }
    female.num_larva++;                     // increment total larvae regardless
}

// choose next task of individual based on logistic funtion and phenotype choice.
void Nest::task_check(const Eigen::MatrixXd& tmp_matrix, Individual<2>& current, const params& p) {

    int total_larvae = larval_females.size() + larval_males.size();
    double foraging_propensity = evaluate_spline2d(tmp_matrix, current.choice_phenotype, n_resolution, total_larvae);
    if (bernoulli(logistic(foraging_propensity))) {
        current.is_foraging = true;
    }
    
    current.t_next = current.t_next + (current.is_foraging ? p.dForagingTime : p.dBroodingTime);
}

// Function to search by indidivual ID in a nest and return index
size_t Nest::findFemaleIndexById(const int id) {
    auto it = std::find_if(adult_females.begin(), adult_females.end(), [id](const Individual<2>& ind) {
        return ind.ind_id == id;
    });

    if (it != adult_females.end()) {
        return std::distance(adult_females.begin(), it);
    } else {
        // Return some sentinel value  to indicate not found
        return -1;
    }
}

// Last of Us
// Function for intial output experiment on last generation
// Mates females with one of the larval males
void Nest::mate_withinNest(Individual<2>& female) {
    std::tuple<bool, size_t> index_tuple = choose_RndmMaleLarva();
    if (std::get<0>(index_tuple)){
        female.mate(larval_males[std::get<1>(index_tuple)]);
    }
}

// Last of Us
// returns index of selected random larval male
std::tuple<bool, size_t> Nest::choose_RndmMaleLarva(){
    if (larval_males.size() != 0) {
        size_t index = uni_int(0, larval_males.size());
        return std::make_tuple(true, index);
    }
    else return std::make_tuple(false, 0);
}

// Function for LastOfUs output
// void Nest::printIndividualEventCSV(const std::vector< float >& param_values, std::ostream& csv_file, const Individual<2>& current, const Individual<2>& recent, const unsigned long int event) {
//     // csv_file << "event,gtime,nest_id,ind_id,mom_id,dad_id,is_mated,mate_id,current_foraging,next_foraging,num_choices,num_foraging,,num_female_larva,num_larva,t_birth,is_alive,t_death,dispersal,choice_int,choice_slope,num_femlarva,num_malelarva,num_adults"<<std::endl;

//     for (auto i : param_values) {
//       csv_file << i << ',';
//     }
    
//     csv_file << event << "," << gtime << "," << currsent.nest_id << "," << current.ind_id << ","
//               << current.mom_id << "," << current.dad_id << "," << current.is_mated << ","
//               << current.mate_id << "," << current.is_foraging << "," << recent.is_foraging << ","
//               << current.num_choices << "," << current.num_foraging << ","
//               << current.num_female_larva << "," << current.num_larva << "," << current.t_birth << ","
//               << current.is_alive << "," << current.t_death << "," << current.phenotype_dispersal << ","
//               << current.phenotype_choice[0] << "," << current.phenotype_choice[1] << "," << larval_females.size() << ","  
//               << larval_males.size() << "," << adult_females.size() << std::endl;
// }

#endif /* Nest_hpp */

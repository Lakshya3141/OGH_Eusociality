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
#include <tuple>

class Nest {
public:
    Nest(const Individual<2>& f, const int nid) ;
    
    // feed function that chooses random larva and feeds it in the nest
    std::tuple<bool, bool, size_t> feed(const double& mean, const double& SD);
    void reproduce(Individual<2>& female, const int id, const params& p);
    void task_check(Individual<2>& current, const params& p);
    size_t findFemaleIndexById(unsigned int id);

    unsigned int nest_id;
    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males
};

// < is there any larvae fed, female == true, index of larvae fed > : Structure of tuple returned from feed function
// 
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
void Nest::task_check(Individual<2>& current, const params& p) {
    current.is_foraging = bernoulli(logistic(larval_females.size() + larval_males.size(), current.phenotype_choice[0], current.phenotype_choice[1]));
    current.t_next = current.t_next + (current.is_foraging ? p.dForagingTime : p.dBroodingTime) + uni_real(); 
    // Adding noise by adding uni_real();    
}

// Function to search by indidivual ID in a nest and return index
size_t Nest::findFemaleIndexById(unsigned int id) {
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


#endif /* Nest_hpp */

//
//  Nest.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 13/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Nest_hpp
#define Nest_hpp

#include "Individual.hpp"

class Nest {
public:
    Nest(Individual<2>& f);
    Nest(Individual<2>&& f);
    
    std::vector<Individual<2>> females; // vector of females, [0] = breeder

    void forage(const params& p);
    double divide_resources(bool is_first_brood);
    int calc_num_males(bool is_first_brood, int num_daughters);
    
    void remove_dead_females();
    bool check_solitary();
    bool is_solitary = true;
    bool is_empty = false;
    double nest_resources = 0.0;
};

Nest::Nest(Individual<2>& f) : females{f} {} // initialiser list for nests (initialised with one female)
Nest::Nest(Individual<2>&& f) : females{f} {} // initialiser list for nests (initialised with one female)

// to determine if nests are social or solitary
bool Nest::check_solitary() {
    if (females.size() == 1) is_solitary = true; // 1 female = solitary
        else is_solitary = false;                // more than one female = social
    return is_solitary;
}

// remove females from nests if dead
void Nest::remove_dead_females() {
    std::erase_if(females,[](auto& f){ return !f.is_alive;});
}

void Nest::forage(const params& p) {
    // breeder forages
    nest_resources = (p.rec_sl_n_breeder * ((females[0].body_size-p.rec_min_bodysize_breeder)*(females[0].body_size-p.rec_min_bodysize_breeder))) / (p.rec_int_breeder + p.rec_sl_d_breeder * ((females[0].body_size-p.rec_min_bodysize_breeder)*(females[0].body_size-p.rec_min_bodysize_breeder)));
    
    // foraging returns = 0.0 for body sizes below minimum
    if (females[0].body_size < p.rec_min_bodysize_breeder) {
        nest_resources = 0.0;
    }
    
    // potential helpers forage
    for (size_t i=1; i<females.size(); ++i) {
        double temp_resources = (p.rec_sl_n_helper * ((females[i].body_size-p.rec_min_bodysize_helper)*(females[i].body_size-p.rec_min_bodysize_helper))) / (p.rec_int_helper + p.rec_sl_d_helper * ((females[i].body_size-p.rec_min_bodysize_helper)*(females[i].body_size-p.rec_min_bodysize_helper)));
        
        // foraging returns = 0.0 for body sizes below minimum
        if (females[i].body_size < p.rec_min_bodysize_helper) {
            temp_resources = 0.0;
        }
        nest_resources += temp_resources; // add helper foraging returns to total nest resources
    }
}

// total nest resources divided according to sex allocation genes
double Nest::divide_resources(bool is_first_brood) {
    double fem_resources = 0.0;
    
    if (is_first_brood) {
        fem_resources = (1-logistic(females[0].phenotype_sex_allocations[0])) * nest_resources;
    }
    else {
        fem_resources = (1-logistic(females[0].phenotype_sex_allocations[1])) * nest_resources;
    }
    
    return fem_resources;
}

#endif /* Nest_hpp */
